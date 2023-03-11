//
//  IRAnalysisController.cpp
//  BodeAnalyzer_App
//
//  created by yu2924 on 2023-02-05
//

#include "IRAnalysisController.h"
#include "SimpleFFT.h"
#include "SimpleConv.h"
#include "MLS.h"

namespace BAAPP
{

	namespace IR
	{

		class TemporaryFile : public juce::ReferenceCountedObject
		{
		protected:
			const juce::File path;
			int numChannels = 0;
			int repeatCount = 0;
			std::unique_ptr<juce::FileOutputStream> outputStr;
			std::unique_ptr<juce::FileInputStream> inputStr;
		public:
			using Ptr = juce::ReferenceCountedObjectPtr<TemporaryFile>;
			TemporaryFile() : path(juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile("BAAPP-IR.tmp"))
			{
				DBG("IR::TemporaryFile: path=" << path.getFullPathName().quoted());
			}
			virtual ~TemporaryFile()
			{
				outputStr = nullptr;
				inputStr = nullptr;
				if(path.exists()) path.deleteFile();
			}
			int getNumChannels() const { return numChannels; }
			int getRepeatCount() const { return repeatCount; }
			bool isWriting() const { return outputStr != nullptr; }
			bool isReading() const { return inputStr != nullptr; }
			juce::Result beginWrite(int cch, int repeat)
			{
				if(inputStr) return juce::Result::fail("tmpfile still reading");
				outputStr = std::make_unique<juce::FileOutputStream>(path);
				if(outputStr->failedToOpen()) { outputStr = nullptr; return juce::Result::fail("tmpfile open error"); }
				numChannels = cch;
				repeatCount = repeat;
				outputStr->setPosition(0);
				outputStr->truncate();
				outputStr->writeInt(numChannels);
				outputStr->writeInt(repeatCount);
				return juce::Result::ok();
			}
			void endWrite()
			{
				if(outputStr)
				{
					DBG("IR::TemporaryFile: writtenframes=" << (outputStr->getPosition() - 2 * sizeof(int)) / (numChannels * sizeof(float)));
					outputStr->flush();
				}
				outputStr = nullptr;
			}
			bool write(const float* const* pp, int cch, int off, int len)
			{
				if(!outputStr || (cch != numChannels)) return false;
				for(int i = 0; i < len; ++i) { for(int ich = 0; ich < cch; ++ich) outputStr->writeFloat(pp[ich][i + off]); }
				return true;
			}
			juce::Result beginRead()
			{
				if(outputStr) return juce::Result::fail("tmpfile still writing");
				inputStr = std::make_unique<juce::FileInputStream>(path);
				if(inputStr->failedToOpen()) { inputStr = nullptr; return juce::Result::fail("tmpfile open error"); }
				int cch = inputStr->readInt();
				int repeat = inputStr->readInt();
				if((cch != numChannels) || (repeat != repeatCount)) { inputStr = nullptr; return juce::Result::fail("tmpfile property mismatch"); }
				return juce::Result::ok();
			}
			void endRead()
			{
				inputStr = nullptr;
			}
			bool read(float* const* pp, int cch, int len, bool accumulate)
			{
				if(!inputStr || (cch != numChannels)) return false;
				if(accumulate)	{ for(int i = 0; i < len; ++i) { for(int ich = 0; ich < cch; ++ich) pp[ich][i] += inputStr->readFloat(); } }
				else			{ for(int i = 0; i < len; ++i) { for(int ich = 0; ich < cch; ++ich) pp[ich][i] = inputStr->readFloat(); } }
				return true;
			}
		};

		// ================================================================================
		// AnalysisWorker

		class AnalysisWorker : public juce::ReferenceCountedObject
		{
		public:
			using Ptr = juce::ReferenceCountedObjectPtr<AnalysisWorker>;
			virtual int getStimulusLength() const = 0;
			virtual int getExtraRepeatCount() const = 0;
			virtual std::vector<float> setupAndGenerateStimulus() = 0;
			virtual juce::Result analyze(TemporaryFile::Ptr tmpfile, std::vector<AnalysisController::Response::Channel>* respchlist) = 0;
		};

		// --------------------------------------------------------------------------------

		class ImpulseAnalysisWorker : public AnalysisWorker
		{
		protected:
			const int order = 0;
			const float amplitude = 1;
		public:
			ImpulseAnalysisWorker(int ord, float a) : order(ord), amplitude(a) {}
			virtual int getStimulusLength() const override { return 1 << order; }
			virtual int getExtraRepeatCount() const override { return 0; }
			virtual std::vector<float> setupAndGenerateStimulus() override
			{
				const int N = 1 << order;
				std::vector<float> stim(N);
				stim[0] = amplitude;
				return stim;
			}
			virtual juce::Result analyze(TemporaryFile::Ptr tmpfile, std::vector<AnalysisController::Response::Channel>* respchlist) override
			{
				const int cch = tmpfile->getNumChannels();
				const int repeat = tmpfile->getRepeatCount();
				const int N = 1 << order;
				DBG(juce::String::formatted("IR::ImpulseAnalysisWorker: cch=%d repeat=%d N=%d", cch, repeat, N));
				juce::AudioSampleBuffer rdbuf(cch, N);
				rdbuf.clear();
				juce::Result r = tmpfile->beginRead();
				if(r.failed()) return r;
				for(int i = 0; i < repeat; ++i) tmpfile->read(rdbuf.getArrayOfWritePointers(), cch, N, true);
				tmpfile->endRead();
				rdbuf.applyGain(1 / (float)repeat);
				SimpleFFTD fft(N);
				for(int ich = 0; ich < cch; ++ich) analyzeChannel(fft, rdbuf.getReadPointer(ich), N, &respchlist->at(ich));
				return juce::Result::ok();
			}
		protected:
			void analyzeChannel(const SimpleFFTD& fft, const float* pbuf, int N, AnalysisController::Response::Channel* respch) const
			{
				respch->ir = std::vector<float>(pbuf, pbuf + N);
				respch->FR.resize(N);
				float gainrecover = 1 / amplitude;
				for(int i = 0; i < N; ++i) respch->ir[i] *= gainrecover;
				for(int i = 0; i < N; ++i) respch->FR[i] = { respch->ir[i], 0 };
				fft.Transform(true, respch->FR.data());
			}
		};

		// --------------------------------------------------------------------------------

		class SweptSineAnalysisWorker : public AnalysisWorker
		{
		protected:
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
				const double a = 4 * (double)m * PI / sq((double)N);
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
				// H(n) = H'(N - n)                     ...(N/2 < n < N)
				// a = 2 m pi / ((N / 2) log(N / 2))
				//
				std::vector<std::complex<double> > vr(N);
				const std::complex<double> j{ 0, 1 };
				const double a = 4 * (double)m * PI / (N * std::log((double)N / 2));
				int n = 0;
				vr[n] = { 1, 0 }; ++n;
				for(; n <= N / 2; ++n) vr[n] = std::exp(-j * a * (double)n * std::log((double)n)) / std::sqrt((double)n);
				for(; n < N; ++n) vr[n] = std::conj(vr[N - n]);
				return vr;
			}
			const int order = 0;
			const float amplitude = 1;
			std::vector<std::complex<double> > INVTSP;
			SimpleFFTD fft;
			int rotateShift = 0;
			float gainboost = 1;
			const bool uselogss;
		public:
			SweptSineAnalysisWorker(int ord, float a, bool logss) : order(ord), amplitude(a), uselogss(logss) {}
			virtual int getStimulusLength() const override { return 1 << order; }
			virtual int getExtraRepeatCount() const override { return 0; }
			virtual std::vector<float> setupAndGenerateStimulus() override
			{
				const int N = 1 << order;
				const int m = N / 4; // must be less than N/2
				rotateShift = N / 2 - m;
				gainboost = 1;
				fft.SetSize(N);
				// TSP
				std::vector<std::complex<double> > TSP = uselogss
					? getenerateLogSS(N, m)
					: getenerateLinearSS(N, m);
				// tsp = ifft(TSP)
				std::vector<float> tsp(N);
				{
					std::vector<std::complex<double> > TMP = TSP;
					fft.Transform(false, TMP.data());
					for(int i = 0; i < N; ++i) tsp[i] = (float)TMP[i].real();
					float peak = 0; for(int i = 0; i < N; ++i) peak = std::max(peak, std::abs(tsp[i]));
					gainboost = amplitude / peak;
					for(int i = 0; i < N; ++i) tsp[i] *= gainboost;
					std::rotate(tsp.begin(), tsp.begin() + N - rotateShift, tsp.end());
				}
				// INVTSP = 1 / TSP
				INVTSP.assign(N, { 0, 0 });
				for(int i = 0; i < N; ++i) INVTSP[i] = 1.0 / TSP[i];
				return tsp;
			}
			virtual juce::Result analyze(TemporaryFile::Ptr tmpfile, std::vector<AnalysisController::Response::Channel>* respchlist) override
			{
				const int cch = tmpfile->getNumChannels();
				const int repeat = tmpfile->getRepeatCount();
				const int N = 1 << order;
				DBG(juce::String::formatted("IR::SweptSineAnalysisWorker: cch=%d repeat=%d N=%d", cch, repeat, N));
				juce::AudioSampleBuffer rdbuf(cch, N);
				rdbuf.clear();
				juce::Result r = tmpfile->beginRead();
				if(r.failed()) return r;
				for(int i = 0; i < repeat; ++i) tmpfile->read(rdbuf.getArrayOfWritePointers(), cch, N, true);
				tmpfile->endRead();
				rdbuf.applyGain(1 / (float)repeat);
				for(int ich = 0; ich < cch; ++ich) analyzeChannel(rdbuf.getReadPointer(ich), N, &respchlist->at(ich));
				return juce::Result::ok();
			}
		protected:
			void analyzeChannel(const float* pbuf, int N, AnalysisController::Response::Channel* respch) const
			{
				// TSPRESP = fft(tspresp) * INVTSP
				const float* tspresp = pbuf;
				std::vector<std::complex<double> > TSPRESP(N);
				{
					float gainrecover = 1 / gainboost;
					for(int i = 0; i < N; ++i) TSPRESP[i] = { tspresp[i] * gainrecover, 0 };
					fft.Transform(true, TSPRESP.data());
					for(int i = 0; i < N; ++i) TSPRESP[i] *= INVTSP[i];
				}
				// ir = ifft(TSPRESP)
				std::vector<float>& ir = respch->ir;
				ir.resize(N);
				{
					std::vector<std::complex<double> > TMP = TSPRESP;
					fft.Transform(false, TMP.data());
					for(int i = 0; i < N; ++i) ir[i] = (float)TMP[i].real();
					std::rotate(ir.begin(), ir.begin() + rotateShift, ir.end());
				}
				// FR = fft(ir)
				std::vector<std::complex<double> >& FR = respch->FR;
				FR.resize(N);
				{
					for(int i = 0; i < N; ++i) FR[i] = { ir[i], 0 };
					fft.Transform(true, FR.data());
				}
			}
		};

		// --------------------------------------------------------------------------------

		class MLSAnalysisWorker : public AnalysisWorker
		{
		public:
			static std::vector<float> generateMLS(int order, int len)
			{
				jassert(3 <= order);
				std::vector<float> vr(len);
				MLS mls(order);
				for(int i = 0; i < len; ++i) vr[i] = mls.next() ? 1.0f : -1.0f;
				return vr;
			}
			const int order = 0;
			const float amplitude = 1;
			std::vector<float> inverseStimulus;
			MLSAnalysisWorker(int ord, float a) : order(ord), amplitude(a) {}
			virtual int getStimulusLength() const override { return (1 << order) - 1; }
			virtual int getExtraRepeatCount() const override { return 2; }
			virtual std::vector<float> setupAndGenerateStimulus() override
			{
				const int N = 1 << order;
				const int L = N - 1;
				std::vector<float> mls = generateMLS(order, L);
				std::vector<float> stim = mls; for(auto&& e : stim) e *= amplitude;
				inverseStimulus.resize(N);
				std::reverse_copy(mls.begin(), mls.end(), inverseStimulus.begin() + 1);
				return stim;
			}
			virtual juce::Result analyze(TemporaryFile::Ptr tmpfile, std::vector<AnalysisController::Response::Channel>* respchlist) override
			{
				const int cch = tmpfile->getNumChannels();
				const int repeat = tmpfile->getRepeatCount();
				jassert(3 <= repeat);
				const int N = 1 << order;
				const int L = N - 1;
				DBG(juce::String::formatted("IR::MLSAnalysisWorker: cch=%d repeat=%d L=%d", cch, repeat, L));
				std::vector<std::unique_ptr<SimpleConvF> > convolvers(cch);
				for(auto&& conv : convolvers)
				{
					conv = std::make_unique<SimpleConvF>(N);
					conv->SetIR(inverseStimulus.data(), inverseStimulus.size());
				}
				SimpleFFTD fft(N);
				for(int ich = 0; ich < cch; ++ich)
				{
					AnalysisController::Response::Channel& ch = respchlist->at(ich);
					ch.ir.resize(L);
					ch.FR.resize(N);
				}
				juce::AudioSampleBuffer rdbuf(cch, N);
				rdbuf.clear();
				juce::AudioSampleBuffer irbuf(cch, L);
				irbuf.clear();
				juce::Result r = tmpfile->beginRead();
				if(r.failed()) return r;
				// convolve: initial latency
				tmpfile->read(rdbuf.getArrayOfWritePointers(), cch, N, false);
				for(int ich = 0; ich < cch; ++ich) convolvers[ich]->Process(rdbuf.getWritePointer(ich), N);
				// convolve: discard 2, and accumulate the rest
				for(int i = 0; i < repeat; ++i)
				{
					tmpfile->read(rdbuf.getArrayOfWritePointers(), cch, L, false);
					for(int ich = 0; ich < cch; ++ich)
					{
						convolvers[ich]->Process(rdbuf.getWritePointer(ich), L);
						if(2 <= i) irbuf.addFrom(ich, 0, rdbuf, ich, 0, L, 1);
					}
				}
				irbuf.applyGain(1 / ((float)L * amplitude) * (float)(repeat - 2));
				tmpfile->endRead();
				// result
				for(int ich = 0; ich < cch; ++ich)
				{
					AnalysisController::Response::Channel& ch = respchlist->at(ich);
					juce::FloatVectorOperations::copy(ch.ir.data(), irbuf.getReadPointer(ich), L);
					for(int i = 0; i < L; ++i) ch.FR[i] = { ch.ir[i], 0 };
					fft.Transform(true, ch.FR.data());
				}
				return juce::Result::ok();
			}
		};

		// ================================================================================
		// Transmitter and Receiver

		struct Transmitter
		{
			std::vector<float> stimulus;
			int repeatDownCount = 0;
			int readRemaining = 0;
			const std::vector<float>& getStimulus() const { return stimulus; }
			void setStimulus(std::vector<float>& v) { stimulus = v; }
			void prepare(int repeat)
			{
				repeatDownCount = repeat;
				readRemaining = (int)stimulus.size();
			}
			void unprepare()
			{
			}
			void process(float* pdst, int ldst)
			{
				int dstpos = 0;
				while(dstpos < ldst)
				{
					if(repeatDownCount <= 0) break;
					int lseg = std::min(readRemaining, ldst - dstpos);
					juce::FloatVectorOperations::copy(pdst + dstpos, stimulus.data() + stimulus.size() - readRemaining, lseg);
					dstpos += lseg;
					readRemaining -= lseg;
					if(readRemaining <= 0) { --repeatDownCount; readRemaining = (int)stimulus.size(); }
				}
				if(dstpos < ldst)
				{
					juce::FloatVectorOperations::clear(pdst + dstpos, ldst - dstpos);
				}
			}
			bool isCompleted() const
			{
				return repeatDownCount <= 0;
			}
		};

		struct Receiver
		{
			TemporaryFile::Ptr tmpFile;
			int skipRemaining = 0;
			int writeLength = 0;
			int writePosition = 0;
			juce::Result prepare(int cch, int len, int repeat, int latency)
			{
				tmpFile = new TemporaryFile;
				juce::Result r = tmpFile->beginWrite(cch, repeat);
				if(r.failed()) return r;
				skipRemaining = latency;
				writeLength = len * repeat;
				writePosition = 0;
				return juce::Result::ok();
			}
			TemporaryFile::Ptr unprepare()
			{
				if(tmpFile) tmpFile->endWrite();
				TemporaryFile::Ptr tmp = tmpFile;
				tmpFile = nullptr;
				return tmp;
			}
			void process(const float* const* ppsrc, int cch, int srclen)
			{
				jassert(tmpFile != nullptr);
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
					if(writeLength <= writePosition) break;
					int lseg = std::min(writeLength - writePosition, srclen - srcpos);
					tmpFile->write(ppsrc, cch, srcpos, lseg);
					writePosition += lseg;
					srcpos += lseg;
				}
			}
			double getProgress() const
			{
				return (double)writePosition / (double)writeLength;
			}
			bool isCompleted() const
			{
				return writeLength <= writePosition;
			}
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
				parameters.method		= std::max(MethodImpulse, std::min(MethodMLS, v.method));
				parameters.order		= std::max(16, std::min(20, v.order));
				parameters.amplitude	= std::max(1e-3f, v.amplitude);
				parameters.repeatCount	= std::max(1, std::min(8, v.repeatCount));
				analysisWorker = nullptr;
				switch(parameters.method)
				{
					case MethodImpulse	: analysisWorker = new ImpulseAnalysisWorker	(parameters.order, parameters.amplitude); break;
					case MethodLinearSS	: analysisWorker = new SweptSineAnalysisWorker	(parameters.order, parameters.amplitude, false); break;
					case MethodLogSS	: analysisWorker = new SweptSineAnalysisWorker	(parameters.order, parameters.amplitude, true); break;
					case MethodMLS		: analysisWorker = new MLSAnalysisWorker		(parameters.order, parameters.amplitude); break;
					default: break; // unexpected
				}
				std::vector<float> stimulus = analysisWorker->setupAndGenerateStimulus();
				transmitter.setStimulus(stimulus);
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
				return transmitter.getStimulus();
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
				double fs = dev->getCurrentSampleRate();
				if(fs <= 0) return juce::Result::fail("unexpected samplerate");
				int ccho = dev->getActiveOutputChannels().countNumberOfSetBits();
				int cchi = dev->getActiveInputChannels().countNumberOfSetBits();
				if((ccho <= 0) || (cchi <= 0)) return juce::Result::fail("device channel disabled");
				int length = analysisWorker->getStimulusLength();
				int repeat = parameters.repeatCount + analysisWorker->getExtraRepeatCount();
				juce::Result r = receiver.prepare(cchi, length, repeat, roundtripLatency);
				if(r.failed()) return r;
				transmitter.prepare(repeat);
				sessionProgress = 0;
				sessionRunning = true;
				callProgressiveSessionBegin();
				clearResponse();
				response.sampleRate = fs;
				response.channelList.resize(cchi);
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
					transmitter.unprepare();
					receiver.unprepare();
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
						transmitter.unprepare();
						TemporaryFile::Ptr tmpfile = receiver.unprepare();
						juce::Thread::launch([this, tmpfile]()
						{
							double t = 0;
							{
								juce::ScopedTimeMeasurement stm(t);
								juce::Result r = analysisWorker->analyze(tmpfile, &response.channelList);
								juce::MessageManager::callAsync([this, r]() { callProgressiveSessionEnd(r, false); });
							}
							DBG("IR::AnalysisController: method=" << parameters.method << " elapsedtime=" << t * 1000 << " ms");
						});
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
					transmitter.unprepare();
					receiver.unprepare();
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
					transmitter.unprepare();
					receiver.unprepare();
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
