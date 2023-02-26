//
//  IRAnalysisController.cpp
//  BodeAnalyzer_App
//
//  created by yu2924 on 2023-02-05
//

#include "IRAnalysisController.h"
#include "SimpleFFT.h"

namespace BAAPP
{

	namespace IR
	{

		// ================================================================================
		// AnalysisWorker

		class AnalysisWorker : public juce::ReferenceCountedObject
		{
		public:
			using Ptr = juce::ReferenceCountedObjectPtr<AnalysisWorker>;
			size_t length = 0;
			float amplitude = 1;
			AnalysisWorker(size_t l, float a) : length(l), amplitude(a) {}
			virtual void prepare(std::vector<float>& stimulus) = 0;
			virtual void analyze(const float* presp, size_t lresp, std::vector<float>& ir, std::vector<std::complex<double> >& FR) const = 0;
		};

		class ImpulseAnalysisWorker : public AnalysisWorker
		{
		public:
			SimpleFFTD fftd;
			ImpulseAnalysisWorker(size_t l, float a) : AnalysisWorker(l, a) {}
			virtual void prepare(std::vector<float>& stimulus) override
			{
				jassert(0 < length);
				stimulus.assign(length, 0);
				stimulus[0] = amplitude;
				fftd.SetSize(length);
			}
			virtual void analyze(const float* presp, size_t lresp, std::vector<float>& ir, std::vector<std::complex<double> >& FR) const override
			{
				ir.resize(length);
				std::copy(presp, presp + std::min(length, lresp), ir.data());
				FR.resize(length);
				float gainrecover = 1 / amplitude;
				for(size_t c = length, i = 0; i < c; ++i) ir[i] *= gainrecover;
				for(size_t c = length, i = 0; i < c; ++i) FR[i] = { ir[i], 0 };
				fftd.Transform(true, FR.data());
			}
		};

		class SweptSineAnalysisWorker : public AnalysisWorker
		{
		public:
			static constexpr double PI = juce::MathConstants<double>::pi;
			template<typename T> static T sq(T v) { return v * v; }
			static std::vector<std::complex<double> > getenerateLinearSS(int N, int m)
			{
				// OATSP
				//
				// H(n) = exp(-j 4 m pi n^2 / N^2)  ...(0 <= n <= N/2)
				// H(n) = H'(N - n)                 ...(N/2 < n < N)
				// m < N/2
				//
				std::vector<std::complex<double> > vr(N);
				const std::complex<double> j{ 0, 1 };
				const double a = 4.0 * (double)m * PI / sq((double)N);
				int n = 0;
				for(; n <= N / 2; ++n) vr[n] = std::exp(-j * a * sq((double)n));
				for(; n < N; ++n) vr[n] = std::conj(vr[N - n]);
				return vr;
			}
			static std::vector<std::complex<double> > getenerateLogSS(int N, int m)
			{
				// Log TSP (Pink TSP)
				//
				// H(n) = 1                             ...(n = 0)
				// H(n) = exp(j a n log(n)) / sqrt(n)   ...(0 < n <= N/2)
				// H(n) = H(N - n)                      ...(N/2 < n < N)
				// a = 2 m pi / ((N / 2) log(N / 2))
				//
				std::vector<std::complex<double> > vr(N);
				const std::complex<double> j{ 0, 1 };
				const double a = 4 * m * PI / (N * std::log((double)N / 2));
				int n = 0;
				vr[n] = { 1, 0 }; ++n;
				// for(; n <= N / 2; ++n) vr[n] = std::exp(-j * a * (double)n * std::log((double)n)) / std::sqrt((double)n); // ???
				for(; n <= N / 2; ++n) vr[n] = std::exp(-j * a * (double)(n + 1) * std::log((double)(n + 1))) / std::sqrt((double)(n + 1));
				for(; n < N; ++n) vr[n] = std::conj(vr[N - n]);
				return vr;
			}
			std::vector<std::complex<double> > INVTSP;
			SimpleFFTD fftd;
			int rotateShift = 0;
			float gainboost = 1;
			const bool uselogss;
			SweptSineAnalysisWorker(size_t l, float a, bool logss) : AnalysisWorker(l, a), uselogss(logss)
			{
			}
			virtual void prepare(std::vector<float>& stimulus) override
			{
				jassert(0 < length);
				int N = (int)length;
				int m = N / 4; // must be less than N/2
				rotateShift = N / 2 - m;
				gainboost = 1;
				fftd.SetSize(length);
				// TSP
				std::vector<std::complex<double> > TSP = uselogss
					? getenerateLogSS(N, m)
					: getenerateLinearSS(N, m);
				// tsp: ifft(TSP)
				std::vector<float>& tsp = stimulus;
				{
					tsp.resize(N, 0);
					std::vector<std::complex<double> > TMP = TSP;
					fftd.Transform(false, TMP.data());
					for(int i = 0; i < N; ++i) tsp[i] = (float)TMP[i].real();
					float peak = 0; for(int i = 0; i < N; ++i) peak = std::max(peak, std::abs(tsp[i]));
					gainboost = amplitude / peak;
					for(int i = 0; i < N; ++i) tsp[i] *= gainboost;
					std::rotate(tsp.begin(), tsp.begin() + N - rotateShift, tsp.end());
				}
				// INVTSP: 1/TSP (or TSP.conj())
				INVTSP.assign(N, { 0, 0 });
				for(int i = 0; i < N; ++i) INVTSP[i] = 1.0 / TSP[i];
			}
			virtual void analyze(const float* presp, size_t lresp, std::vector<float>& ir, std::vector<std::complex<double> >& FR) const override
			{
				int N = (int)length;
				// TSPRESP: fft(tspresp)*INVTSP
				std::vector<float> tspresp(N, 0);
				std::copy(presp, presp + std::min(length, lresp), tspresp.data());
				tspresp.resize(N, 0);
				std::vector<std::complex<double> > TSPRESP(N);
				{
					float gainrecover = 1 / gainboost;
					for(int i = 0; i < N; ++i) TSPRESP[i] = { tspresp[i] * gainrecover, 0 };
					fftd.Transform(true, TSPRESP.data());
					for(int i = 0; i < N; ++i) TSPRESP[i] *= INVTSP[i];
				}
				// ir: ifft(TSPRESP)
				ir.resize(N);
				{
					std::vector<std::complex<double> > TMP = TSPRESP;
					fftd.Transform(false, TMP.data());
					for(int i = 0; i < N; ++i) ir[i] = (float)TMP[i].real();
					std::rotate(ir.begin(), ir.begin() + rotateShift, ir.end());
				}
				// FR: fft(ir)
				FR.resize(N);
				{
					for(int i = 0; i < N; ++i) FR[i] = { ir[i], 0 };
					fftd.Transform(true, FR.data());
				}
			}
		};

		// ================================================================================
		// Transmitter and Receiver

		struct Transmitter
		{
			std::vector<float> buffer;
			int repeatDownCount = 0;
			int readRemaining = 0;
			void setSize(int len)
			{
				buffer.resize(len, 0);
			}
			void prepare(int repeat)
			{
				repeatDownCount = repeat;
				readRemaining = (int)buffer.size();
			}
			void process(float* pdst, int ldst)
			{
				while(0 < ldst)
				{
					if(repeatDownCount <= 0) break;
					int lseg = std::min(readRemaining, ldst);
					juce::FloatVectorOperations::copy(pdst, buffer.data() + buffer.size() - readRemaining, lseg);
					ldst -= lseg;
					pdst += lseg;
					readRemaining -= lseg;
					if(readRemaining <= 0) { --repeatDownCount; readRemaining = (int)buffer.size(); }
				}
				if(0 < ldst)
				{
					juce::FloatVectorOperations::clear(pdst, ldst);
				}
			}
			bool isCompleted() const
			{
				return repeatDownCount <= 0;
			}
		};

		struct Receiver
		{
			juce::AudioBuffer<float> buffer;
			int skipRemaining = 0;
			int repeatTotalCount = 0;
			int repeatCount = 0;
			int writeLength = 0;
			int writePosition = 0;
			void setSize(int cch, int len)
			{
				buffer.setSize(cch, len);
				writeLength = len;
			}
			void prepare(int repeat, int latency)
			{
				buffer.clear();
				skipRemaining = latency;
				repeatTotalCount = repeat;
				repeatCount = 0;
				writePosition = 0;
			}
			void process(const float* const* ppsrc, int cch, int srclen)
			{
				int srcpos = 0;
				if(0 < skipRemaining)
				{
					int lseg = std::min(skipRemaining, srclen);
					srcpos += lseg;
					skipRemaining -= lseg;
				}
				if(0 < skipRemaining) return;
				while(srcpos < srclen)
				{
					if(repeatTotalCount <= repeatCount) break;
					int lseg = std::min(buffer.getNumSamples() - writePosition, srclen - srcpos);
					for(int ich = 0; ich < cch; ++ich)
					{
						juce::FloatVectorOperations::add(buffer.getWritePointer(ich, writePosition), ppsrc[ich] + srcpos, lseg);
					}
					writePosition += lseg;
					srcpos += lseg;
					if(buffer.getNumSamples() <= writePosition) { repeatCount++; writePosition = 0; }
				}
			}
			void finalize()
			{
				buffer.applyGain(1.0f / (float)repeatTotalCount);
			}
			double getProgress() const { return (double)(writeLength * repeatCount + writePosition) / (double)(writeLength * repeatTotalCount); }
			bool isCompleted() const { return repeatTotalCount <= repeatCount; }
		};

		// ================================================================================
		// AnalysisController

		class AnalysisControllerImpl : public AnalysisController, public juce::AudioIODeviceCallback
		{
		public:
			juce::AudioDeviceManager* audioDeviceManager;
			Parameters parameters = {};
			AnalysisWorker::Ptr analysisWorker;
			Transmitter transmitter;
			Receiver receiver;
			Response response = {};
			int roundtripLatency = 0;
			double sessionProgress = 0;
			bool sessionRunning = false;
			AnalysisControllerImpl(juce::AudioDeviceManager* adm) : audioDeviceManager(adm)
			{
				response.sampleRate = 44100;
			}
			virtual ~AnalysisControllerImpl()
			{
				jassert(!sessionRunning);
				audioDeviceManager->removeAudioCallback(this);
			}
			// --------------------------------------------------------------------------------
			// FastAnalysisController
			virtual const Parameters& getParameters() const override
			{
				return parameters;
			}
			virtual void setParameters(const Parameters& v) override
			{
				parameters.method = std::max(MethodImpulse, std::min(MethodLogSS, v.method));
				parameters.order = std::max(16, std::min(20, v.order));
				parameters.amplitude = std::max(1e-3f, v.amplitude);
				parameters.repeatCount = std::max(1, std::min(8, v.repeatCount));
				int length = 1 << parameters.order;
				float amplitude = parameters.amplitude;
				transmitter.setSize(length);
				analysisWorker = nullptr;
				switch(parameters.method)
				{
					case MethodImpulse: analysisWorker = new ImpulseAnalysisWorker(length, amplitude); break;
					case MethodLinearSS: analysisWorker = new SweptSineAnalysisWorker(length, amplitude, false); break;
					case MethodLogSS: analysisWorker = new SweptSineAnalysisWorker(length, amplitude, true); break;
					default: break; // unexpected
				}
				analysisWorker->prepare(transmitter.buffer);
				sendChangeMessage();
			}
			virtual int getRoundtripLatency() const override
			{
				return roundtripLatency;
			}
			virtual void setRoundtripLatency(int v) override
			{
				roundtripLatency = std::max(0, v);
				sendChangeMessage();
			}
			virtual const std::vector<float>& getStimulus() const override
			{
				return transmitter.buffer;
			}
			virtual const Response& getResponse() const override
			{
				return response;
			}
			virtual void clearResponse() override
			{
				response.channelList.clear();
				response.channelList.shrink_to_fit();
			}
			// --------------------------------------------------------------------------------
			// ProgressiveSessionBase
			virtual juce::Result startProgressiveSession() override
			{
				if(sessionRunning) return juce::Result::fail("session still running");
				auto dev = audioDeviceManager->getCurrentAudioDevice();
				if(!dev) return juce::Result::fail("device not ready");
				int ccho = dev->getActiveOutputChannels().countNumberOfSetBits();
				int cchi = dev->getActiveInputChannels().countNumberOfSetBits();
				if((ccho <= 0) || (cchi <= 0)) return juce::Result::fail("device channel disabled");
				sessionProgress = 0;
				sessionRunning = true;
				callProgressiveSessionBegin();
				clearResponse();
				response.sampleRate = dev->getCurrentSampleRate();
				response.channelList.resize(cchi);
				int length = 1 << parameters.order;
				receiver.setSize(cchi, length);
				receiver.prepare(parameters.repeatCount, roundtripLatency);
				transmitter.prepare(parameters.repeatCount);
				audioDeviceManager->addAudioCallback(this);
				return juce::Result::ok();
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
			virtual bool isProgressiveSessionRunning() const override
			{
				return sessionRunning;
			}
			virtual double getProgressiveSessionProgress() const override
			{
				return sessionProgress;
			}
			// --------------------------------------------------------------------------------
			// juce::AudioIODeviceCallback
			virtual void audioDeviceIOCallbackWithContext(const float* const* ppi, int cchi, float* const* ppo, int ccho, int len, const juce::AudioIODeviceCallbackContext&)override
			{
				for(int ich = 0; ich < ccho; ++ich) juce::FloatVectorOperations::clear(ppo[ich], len);
				if(transmitter.isCompleted() && receiver.isCompleted()) return;
				transmitter.process(ppo[0], len);
				for(int ich = 1; ich < ccho; ++ich) juce::FloatVectorOperations::copy(ppo[ich], ppo[0], len);
				receiver.process(ppi, cchi, len);
				sessionProgress = receiver.getProgress();
				if(transmitter.isCompleted() && receiver.isCompleted())
				{
					juce::MessageManager::callAsync([this]()
					{
						if(!sessionRunning) return;
						// endsession case: succeeded
						sessionRunning = false;
						audioDeviceManager->removeAudioCallback(this);
						receiver.finalize();
						for(int cch = (int)response.channelList.size(), ich = 0; ich < cch; ++ich)
						{
							analysisWorker->analyze(receiver.buffer.getReadPointer(ich), receiver.buffer.getNumSamples(), response.channelList[ich].ir, response.channelList[ich].FR);
						}
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

		AnalysisController::Ptr AnalysisController::createInstance(juce::AudioDeviceManager* adm)
		{
			return new AnalysisControllerImpl(adm);
		}

	} // namespace IR

} // namespace BAAPP
