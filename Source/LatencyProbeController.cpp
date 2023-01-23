//
//  LatencyProbeController.cpp
//  BodeAnalyzer_App
//
//  created by yu2924 on 2023-02-09
//

#include "LatencyProbeController.h"
#include "SimpleFFT.h"
#include <random>

namespace BAAPP
{

	class LatencyProbeControllerImpl : public LatencyProbeController, public juce::AudioIODeviceCallback
	{
	public:
		struct CorrelationBuffer
		{
			std::vector<float> txbuffer;
			std::vector<float> rxbuffer;
			int length = 0, position = 0;
			void prepare(float a, int len)
			{
				length = juce::nextPowerOfTwo(len);
				position = 0;
				txbuffer.assign(length, 0.0f);
				rxbuffer.assign(length, 0.0f);
				std::mt19937 mt;
				for(int i = 0; i < len; ++i) txbuffer[i] = (float)(int)mt() / (float)0x7fffffff * a;
			}
			void processBuffer(const float* const* ppi, int cchi, float* const* ppo, int ccho, int len)
			{
				int lseg = std::min(length - position, len);
				if(0 < lseg)
				{
					for(int ich = 0; ich < ccho; ++ich) juce::FloatVectorOperations::copy(ppo[ich], txbuffer.data() + position, lseg);
					for(int ich = 0; ich < cchi; ++ich) juce::FloatVectorOperations::add(rxbuffer.data() + position, ppi[ich], lseg);
					position += lseg;
				}
			}
			double getProgressPosition() const
			{
				return (double)position / (double)length;
			}
			int findDelay() const
			{
				SimpleFFTF fft(length);
				int N = length;
				// a = stimulus, b = response
				const std::vector<float>& a = txbuffer;
				const std::vector<float>& b = rxbuffer;
				// A = fft(a), B = fft(b)
				std::vector<std::complex<float> > A(N);
				for(int i = 0; i < N; ++i) A[i] = { a[i], 0 };
				fft.Transform(true, A.data());
				std::vector<std::complex<float> > B(N);
				for(int i = 0; i < N; ++i) B[i] = { b[i], 0 };
				fft.Transform(true, B.data());
				// C = A * conj(B)
				std::vector<std::complex<float> > C(N);
				for(int i = 0; i < N; ++i) C[i] = A[i] * std::conj(B[i]);
				fft.Transform(false, C.data());
				// c = real(ifft(C)) * (1 / N)
				std::vector<float> c(N);
				float scale = 1 / (float)N;
				for(int i = 0; i < N; ++i) c[i] = C[i].real() * scale;
				// search peak
				std::vector<float>::iterator it = std::max_element(c.begin(), c.end());
				int imax = (int)std::distance(c.begin(), it);
				return N - imax;
			}
			bool isCompleted() const { return length <= position; }
		};
		juce::AudioDeviceManager* audioDeviceManager;
		int probeLength = 4096;
		float probeAmplitude = 0.5f;
		int resultLatency = 0;
		CorrelationBuffer correlationBuffer;
		bool sessionRunning = false;
		LatencyProbeControllerImpl(juce::AudioDeviceManager* adm) : audioDeviceManager(adm)
		{
		}
		virtual ~LatencyProbeControllerImpl()
		{
			audioDeviceManager->removeAudioCallback(this);
		}
		// --------------------------------------------------------------------------------
		// LatencyProbe
		virtual int getProbeLength() const override
		{
			return probeLength;
		}
		virtual void setProbeLength(int v) override
		{
			probeLength = std::max(0, v);
			sendChangeMessage();
		}
		virtual float getProbeAmplitude() const override
		{
			return probeAmplitude;
		}
		virtual void setProbeAmplitude(float v) override
		{
			probeAmplitude = std::max(0.0f, v);
			sendChangeMessage();
		}
		virtual int getResultLatency() const override
		{
			return resultLatency;
		}
		virtual juce::Result startSession() override
		{
			if(sessionRunning) return juce::Result::fail("session still running");
			auto dev = audioDeviceManager->getCurrentAudioDevice();
			if(!dev) return juce::Result::fail("device not ready");
			int ccho = dev->getActiveOutputChannels().countNumberOfSetBits();
			int cchi = dev->getActiveInputChannels().countNumberOfSetBits();
			if((ccho <= 0) || (cchi <= 0)) return juce::Result::fail("device channel disabled");
			correlationBuffer.prepare(probeAmplitude, juce::nextPowerOfTwo(probeLength));
			sessionRunning = true;
			audioDeviceManager->addAudioCallback(this);
			return juce::Result::ok();
		}
		// --------------------------------------------------------------------------------
		// ProgressiveSessionBase
		virtual bool isProgressiveSessionRunning() const override
		{
			return sessionRunning;
		}
		virtual double getProgressiveSessionProgress() const override
		{
			return correlationBuffer.getProgressPosition();
		}
		virtual void abortProgressiveSession() override
		{
			juce::MessageManager::callAsync([this]()
			{
				if(!sessionRunning) return;
				// endsession case: aborted
				sessionRunning = false;
				audioDeviceManager->removeAudioCallback(this);
				callProgressiveSessionEnd(juce::Result::fail("aborted"), true);
			});
		}
		// --------------------------------------------------------------------------------
		// juce::AudioIODeviceCallback
		virtual void audioDeviceIOCallbackWithContext(const float*const* ppi, int cchi, float*const* ppo, int ccho, int len, const juce::AudioIODeviceCallbackContext&) override
		{
			for(int icho = 0; icho < ccho; ++icho) juce::FloatVectorOperations::clear(ppo[icho], len);
			if(correlationBuffer.isCompleted()) return;
			correlationBuffer.processBuffer(ppi, cchi, ppo, ccho, len);
			if(correlationBuffer.isCompleted())
			{
				juce::MessageManager::callAsync([this]()
				{
					if(!sessionRunning) return;
					// endsession case: succeeded
					sessionRunning = false;
					resultLatency = correlationBuffer.findDelay();
					audioDeviceManager->removeAudioCallback(this);
					callProgressiveSessionEnd(juce::Result::ok(), false);
				});
			}
		}
		virtual void audioDeviceAboutToStart(juce::AudioIODevice*) override
		{
		}
		virtual void audioDeviceStopped() override
		{
			juce::MessageManager::callAsync([this]()
			{
				if(!sessionRunning) return;
				// endsession case: unexpected stop
				sessionRunning = false;
				audioDeviceManager->removeAudioCallback(this);
				callProgressiveSessionEnd(juce::Result::fail("unexpected stop"), false);
			});
		}
		virtual void audioDeviceError(const juce::String& msg) override
		{
			juce::MessageManager::callAsync([this, msg]()
			{
				if(!sessionRunning) return;
				// endsession case: device error
				sessionRunning = false;
				audioDeviceManager->removeAudioCallback(this);
				callProgressiveSessionEnd(juce::Result::fail(msg), false);
			});
		}
	};

	LatencyProbeController::Ptr LatencyProbeController::createInstance(juce::AudioDeviceManager* adm)
	{
		return new LatencyProbeControllerImpl(adm);
	}

} // namespace BAAPP
