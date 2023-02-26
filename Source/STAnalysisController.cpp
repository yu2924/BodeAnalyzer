//
//  STAnalysisController.cpp
//  BodeAnalyzer_App
//
//  created by yu2924 on 2023-02-22
//

#include "STAnalysisController.h"
#include "SimpleConv.h"
#include "SimpleLPF.h"

namespace BAAPP
{
	namespace ST
	{

		// ================================================================================
		// primitive blocks

		template<typename T> struct SineOscillatorT
		{
			static constexpr double TWOPI = 3.1415926535897932384626433832795 * 2;
			double freq = 0;
			double phase = 0;
			void SetFreq(double v)
			{
				freq = v;
			}
			void Reset(double ph = 0)
			{
				phase = ph;
			}
			T Process()
			{
				T v = (T)std::sin(phase);
				phase += freq * TWOPI; while(TWOPI <= phase) phase -= TWOPI;
				return v;
			}
			void Process(T* p, size_t l)
			{
				for(size_t i = 0; i < l; ++i) p[i] = Process();
			}
			void Process(T* ps, T* pc)
			{
				*ps = (T)std::sin(phase);
				*pc = (T)std::cos(phase);
				phase += freq * TWOPI; while(TWOPI <= phase) phase -= TWOPI;
			}
			void Process(T* ps, T* pc, size_t l)
			{
				for(size_t i = 0; i < l; ++i) Process(ps + i, pc + i);
			}
		};

		using SineOscillatorF = SineOscillatorT<float>;
		using SineOscillatorD = SineOscillatorT<double>;

		struct ConvLPF
		{
			static constexpr double PI = 3.1415926535897932384626433832795;
			static std::vector<double> createfir(int N, double fc)
			{
				std::vector<double> vr(N);
				double sum = 0;
				for(int i = 0; i < N; ++i)
				{
					double w = (double)(2 * PI * fc * (i - N / 2));
					vr[i] = (i == N / 2) ? 1 : (std::sin(w) / w);
					sum += vr[i];
				}
				for(int i = 0; i < N; ++i) vr[i] /= sum;
				return vr;
			}
			static std::vector<double> createwindow(int N)
			{
				std::vector<double> vr(N);
				for(int i = 0; i < N; ++i)
				{
					double x = (double)i / (double)(N - 1);
					// Blackman
					// vr[i] = 0.42 - 0.5 * std::cos(2 * PI * x) + 0.08 * std::cos(4 * PI * x);
					// Blackman-Harris
					vr[i] = 0.35875 - 0.48829 * cos(2 * PI * x) + 0.14128 * cos(4 * PI * x) - 0.01168 * cos(6 * PI * x);
				}
				return vr;
			}
			static int GetLatencyForFIRLength(int len)
			{
				return (len * 3) / 2;
			}
			SimpleConvD convolver;
			void SetFIRLengthAndCutoffFreq(int l, double fc)
			{
				convolver.SetLength(l);
				std::vector<double> fir = createfir(l, fc);
				std::vector<double> wnd = createwindow(l);
				for(int i = 0; i < l; ++i) fir[i] *= wnd[i];
				convolver.SetIR(fir.data(), fir.size());
			}
			void Reset()
			{
				convolver.Reset();
			}
			void Process(double* p, size_t l)
			{
				convolver.Process(p, l);
			}
		};

#define USE_BESSELCHARACTER 1
		struct CascadedLPF
		{
			static int GetApproxSettlingForCutoff(double fc)
			{
#if USE_BESSELCHARACTER
				return (int)(0.8 / fc + 1);
#else
				return (int)(3.5 / fc + 1);
#endif
			}
			std::array<SimpleLPFD, 4> vfilt;
			void SetCutoffFreq(double fc)
			{
				static const struct { double f, q; } AFQ[] =
				{
#if USE_BESSELCHARACTER
					// Bessel character
					// 1%_settling_time ~= 0.79 / fc
					{ 1.78143, 0.50599 },
					{ 1.83514, 0.55961 },
					{ 1.95645, 0.71085 },
					{ 2.19237, 1.2257  },
#else
					// Butterworth character
					// 1%_settling_time ~= 3.5 / fc
					{ 1, 0.509796 },
					{ 1, 0.601345 },
					{ 1, 0.899976 },
					{ 1, 2.562915 },
#endif
				};
				for(size_t i = 0; i < 4; ++i) vfilt[i].SetFQ(AFQ[i].f * fc, AFQ[i].q);
			}
			void Reset()
			{
				for(auto&& filt : vfilt) filt.Reset();
			}
			void Process(double* p, size_t l)
			{
				for(auto&& filt : vfilt) filt.Process(p, l);
			}
		};

		// ================================================================================
		// Transmitter and Receiver
		//
		// TX:
		//   signal = sin(w*t)
		// RX:
		//   signal = A * sin(w*t + dp)
		//   x:
		//     x = signal * sin(w*t) = (A/2) * (cos(dp) + cos(2*w*t + dp))
		//     x = lowpass(x) * 2 = A * cos(dp)
		//   y:
		//     y = signal * cos(w*t) = (A/2) * (sin(dp) + sin(2*w*t + dp))
		//     y = lowpass(y) * 2 = A * sin(dp)
		//   result:
		//     A = sqrt(x^2 + y^2)
		//     dp = atan2(y, x)
		//

		struct Step
		{
			double freq;
			int length;
			int repeat; // 0,1,2,...
		};

		struct Transmitter
		{
			SineOscillatorF oscillator;
			const std::vector<Step>* stepList = nullptr;
			size_t currentStep = 0;
			int stepPos = 0;
			float amplitude = 1;
			void prepare(float a, const std::vector<Step>& steps)
			{
				stepList = &steps;
				currentStep = 0;
				stepPos = 0;
				amplitude = a;
				oscillator.SetFreq(stepList->at(currentStep).freq);
				oscillator.Reset(0);
			}
			void process(float* pdst, int dstlen)
			{
				jassert(stepList);
				int dstpos = 0;
				while(dstpos < dstlen)
				{
					if(stepList->size() <= currentStep) break;
					const Step& step = stepList->at(currentStep);
					int lseg = std::min(dstlen - dstpos, step.length - stepPos);
					oscillator.Process(pdst + dstpos, lseg);
					juce::FloatVectorOperations::multiply(pdst + dstpos, amplitude, lseg);
					dstpos += lseg;
					stepPos += lseg;
					if(step.length <= stepPos)
					{
						// advance to the next step
						currentStep++;
						stepPos = 0;
						if(currentStep < stepList->size()) oscillator.SetFreq(stepList->at(currentStep).freq);
					}
				}
				if(dstpos < dstlen)
				{
					juce::FloatVectorOperations::clear(pdst + dstpos, dstlen - dstpos);
				}
			}
			bool isCompleted() const
			{
				jassert(stepList);
				return stepList->size() <= currentStep;
			}
		};

		struct Receiver
		{
			static constexpr int UPSAMPLEFACTOR = 2;
			static constexpr int UPSAMPLEFIRLEN = 128;
			static void upsample(const float* ps, double* pd, size_t len)
			{
				for(size_t i = 0; i < len; ++i)
				{
					pd[i * 2] = ps[i] * UPSAMPLEFACTOR;
					pd[i * 2 + 1] = 0;
				}
			}
			SineOscillatorD oscillator;
			struct ChStat
			{
				ConvLPF filterUp;
				CascadedLPF filterX, filterY;
				double lastX = 0, lastY = 0;
				std::vector<AnalysisController::MeasuringPoint>* pointList = nullptr;
			};
			std::vector<ChStat> chStatList;
			std::vector<double> bufferSin, bufferCos, bufferInp, bufferWrk;
			const std::vector<Step>* stepList = nullptr;
			double sampleRate = 0;
			size_t currentStep = 0;
			int stepPos = 0;
			int latencyDowncount = 0;
			float amplitude = 1;
			int GetStepSettlingLengthForFreq(double fosc) const
			{
				return (int)CascadedLPF::GetApproxSettlingForCutoff(fosc * 0.3);
			}
			int GetStepMinimumLength() const
			{
				return ConvLPF::GetLatencyForFIRLength(UPSAMPLEFIRLEN) / UPSAMPLEFACTOR;
			}
			void prepare(double fs, int lbuf, int latency, float a, const std::vector<Step>& steps, std::vector<AnalysisController::Response::Channel>& channels)
			{
				stepList = &steps;
				sampleRate = fs;
				currentStep = 0;
				stepPos = 0;
				latencyDowncount = latency + ConvLPF::GetLatencyForFIRLength(UPSAMPLEFIRLEN) / UPSAMPLEFACTOR;
				amplitude = a;
				oscillator.SetFreq(stepList->at(currentStep).freq / UPSAMPLEFACTOR);
				oscillator.Reset(0);
				size_t cch = channels.size();
				chStatList.resize(cch);
				for(size_t ich = 0; ich < cch; ++ich)
				{
					ChStat& cs = chStatList[ich];
					cs.filterUp.SetFIRLengthAndCutoffFreq(UPSAMPLEFIRLEN, 0.505 / UPSAMPLEFACTOR);
					cs.filterUp.Reset();
					cs.filterX.SetCutoffFreq(stepList->at(currentStep).freq * 0.3 / UPSAMPLEFACTOR);
					cs.filterX.Reset();
					cs.filterY.SetCutoffFreq(stepList->at(currentStep).freq * 0.3 / UPSAMPLEFACTOR);
					cs.filterY.Reset();
					cs.lastX = cs.lastY = 0;
					cs.pointList = &channels[ich].pointList;
				}
				bufferSin.assign(lbuf * UPSAMPLEFACTOR, 0);
				bufferCos.assign(lbuf * UPSAMPLEFACTOR, 0);
				bufferInp.assign(lbuf * UPSAMPLEFACTOR, 0);
				bufferWrk.assign(lbuf * UPSAMPLEFACTOR, 0);
			}
			void process(const float* const* ppsrc, int cch, int lsrc)
			{
				jassert(stepList);
				jassert((size_t)cch == chStatList.size());
				int srcpos = 0;
				if(0 < latencyDowncount)
				{
					int lseg = std::min(latencyDowncount, lsrc - srcpos);
					srcpos += lseg;
					latencyDowncount -= lseg;
				}
				if(0 < latencyDowncount) return;
				while(srcpos < lsrc)
				{
					if(stepList->size() <= currentStep) break;
					const Step& step = stepList->at(currentStep);
					int lseg = std::min(step.length - stepPos, lsrc - srcpos);
					oscillator.Process(bufferSin.data(), bufferCos.data(), lseg * UPSAMPLEFACTOR);
					for(int ich = 0; ich < cch; ++ich)
					{
						ChStat& cs = chStatList[ich];
						upsample(ppsrc[ich] + srcpos, bufferInp.data(), lseg);
						cs.filterUp.Process(bufferInp.data(), lseg * UPSAMPLEFACTOR);
						// x = signal * sin(w*t) = (A/2) * (cos(dp) + cos(2*w*t + dp))
						// x = lowpass(x) * 2 = A * cos(dp)
						juce::FloatVectorOperations::multiply(bufferWrk.data(), bufferInp.data(), bufferSin.data(), lseg * UPSAMPLEFACTOR);
						cs.filterX.Process(bufferWrk.data(), lseg * UPSAMPLEFACTOR);
						cs.lastX = bufferWrk[lseg * UPSAMPLEFACTOR - 1] * 2;
						// y = signal * cos(w*t) = (A/2) * (sin(dp) + sin(2*w*t + dp))
						// y = lowpass(y) * 2 = A * sin(dp)
						juce::FloatVectorOperations::multiply(bufferWrk.data(), bufferInp.data(), bufferCos.data(), lseg * UPSAMPLEFACTOR);
						cs.filterY.Process(bufferWrk.data(), lseg * UPSAMPLEFACTOR);
						cs.lastY = bufferWrk[lseg * UPSAMPLEFACTOR - 1] * 2;
					}
					srcpos += lseg;
					stepPos += lseg;
					if(step.length <= stepPos)
					{
						// store the measured values
						constexpr float RAD2DEG = 180 / juce::MathConstants<float>::pi;
						int rep = step.repeat;
						for(int ich = 0; ich < cch; ++ich)
						{
							const ChStat& cs = chStatList[ich];
							// A = sqrt(x^2 + y^2)
							// dp = atan2(y, x)
							double freqHz = step.freq * sampleRate;;
							float a = (float)std::sqrt(cs.lastX * cs.lastX + cs.lastY * cs.lastY) / amplitude;
							float p = (float)std::atan2(cs.lastY, cs.lastX) * RAD2DEG;
							if(rep == 0) cs.pointList->push_back({ freqHz, a, p });
							AnalysisController::MeasuringPoint& mp = cs.pointList->back();
							mp.amplitude = (mp.amplitude * (float)rep / (float)(rep + 1)) + (a / (float)(rep + 1));
							mp.phaseDeg = (mp.phaseDeg * (float)rep / (float)(rep + 1)) + (p / (float)(rep + 1));
						}
						// advance to the next step
						currentStep++;
						stepPos = 0;
						if(currentStep < stepList->size())
						{
							oscillator.SetFreq(stepList->at(currentStep).freq / UPSAMPLEFACTOR);
							for(size_t ich = 0; ich < cch; ++ich)
							{
								ChStat& cs = chStatList[ich];
								cs.filterX.SetCutoffFreq(stepList->at(currentStep).freq * 0.3 / UPSAMPLEFACTOR);
								cs.filterY.SetCutoffFreq(stepList->at(currentStep).freq * 0.3 / UPSAMPLEFACTOR);
								cs.lastX = cs.lastY = 0;
							}
						}
					}
				}
			}
			double getProgress() const
			{
				jassert(stepList);
				return (double)currentStep / (double)stepList->size();
			}
			bool isCompleted() const
			{
				jassert(stepList);
				return stepList->size() <= currentStep;
			}
		};

		// ================================================================================
		// AnalysisController

		class AnalysisControllerImpl : public AnalysisController, public juce::AudioIODeviceCallback
		{
		public:
			static std::vector<double> linspace(double begin, double end, int num)
			{
				std::vector<double> vr(num);
				for(int i = 0; i < num; ++i)
				{
					vr[i] = (double)i / (double)(num - 1) * (end - begin) + begin;
				}
				return vr;
			}
			static std::vector<double> logspace(double begin, double end, int num)
			{
				std::vector<double> vr(num);
				double rng = std::log(end) - std::log(begin);
				for(int i = 0; i < num; ++i)
				{
					vr[i] = std::exp((double)i / (double)(num - 1) * rng) * begin;
				}
				return vr;
			}
			juce::AudioDeviceManager* audioDeviceManager;
			Parameters parameters = {};
			std::vector<Step> stepList;
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
			// AnalysisController
			virtual const Parameters& getParameters() const override
			{
				return parameters;
			}
			virtual void setParameters(const Parameters& v) override
			{
				juce::AudioIODevice* dev = audioDeviceManager->getCurrentAudioDevice();
				double fs = dev ? dev->getCurrentSampleRate() : 44100;
				parameters.freqMinHz = std::max(10.0, v.freqMinHz);
				parameters.freqMaxHz = std::max(10.0, std::min(fs * 0.5, v.freqMaxHz));
				parameters.numPoints = std::max(2, std::min(4096, v.numPoints));
				parameters.amplitude = std::max(1e-3f, v.amplitude);
				parameters.repeatCount = std::max(1, std::min(16, v.repeatCount));
				parameters.logarithmic = v.logarithmic;
				sendChangeMessage();
			}
			virtual int getRoundtripLatency() const override
			{
				return roundtripLatency;
			}
			virtual void setRoundtripLatency(int v) override
			{
				roundtripLatency = v;
				sendChangeMessage();
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
				if(parameters.freqMaxHz <= parameters.freqMinHz) return juce::Result::fail("invalid frequency range");
				sessionProgress = 0;
				sessionRunning = true;
				callProgressiveSessionBegin();
				clearResponse();
				double fs = dev->getCurrentSampleRate();
				int lbuf = dev->getCurrentBufferSizeSamples();
				response.sampleRate = fs;
				response.channelList.resize(cchi);
				for(size_t ich = 0; ich < cchi; ++ich)
				{
					response.channelList[ich].pointList.resize(0);
					response.channelList[ich].pointList.reserve(parameters.numPoints);
				}
				std::vector<double> vstepfreqhz = parameters.logarithmic
					? logspace(parameters.freqMinHz, parameters.freqMaxHz, parameters.numPoints)
					: linspace(parameters.freqMinHz, parameters.freqMaxHz, parameters.numPoints);
				stepList.resize(0);
				stepList.reserve(parameters.numPoints * parameters.repeatCount);
				for(size_t c = parameters.numPoints, i = 0; i < c; ++i)
				{
					double freq = vstepfreqhz[i] / fs;
					int lsettling = receiver.GetStepSettlingLengthForFreq(freq);
					int lstepmin = receiver.GetStepMinimumLength();
					int lcyc10 = (int)(10 / freq + 1);
					for(int rep = 0; rep < parameters.repeatCount; ++rep)
					{
						int length = std::max(((rep == 0) ? lsettling : 0) + lstepmin, lcyc10);
						stepList.push_back({ freq, length, rep });
					}
				}
				transmitter.prepare(parameters.amplitude, stepList);
				receiver.prepare(fs, lbuf, roundtripLatency, parameters.amplitude, stepList, response.channelList);
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

	} // namespace ST
} // namespace BAAPP
