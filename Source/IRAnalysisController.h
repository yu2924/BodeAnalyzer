//
//  IRAnalysisController.h
//  BodeAnalyzer_App
//
//  created by yu2924 on 2023-02-05
//

#pragma once

#include "ProgressiveSessionBase.h"
#include <vector>
#include <complex>

namespace BAAPP
{
	namespace IR
	{

		class AnalysisController : public ProgressiveSessionBase, public juce::ChangeBroadcaster
		{
		protected:
			AnalysisController() {}
		public:
			using Ptr = juce::ReferenceCountedObjectPtr<AnalysisController>;
			enum Method { MethodImpulse, MethodLinearSS, MethodLogSS, MethodMLS };
			struct Parameters
			{
				Method method;
				int order; // length = 1 << order
				float amplitude;
				int repeatCount;
			};
			struct Response
			{
				double sampleRate;
				struct Channel
				{
					std::vector<float> ir;
					std::vector<std::complex<double> > FR;
				};
				std::vector<Channel> channelList;
			};
			virtual const Parameters& getParameters() const = 0;
			virtual void setParameters(const Parameters& v) = 0;
			virtual int getRoundtripLatency() const = 0;
			virtual void setRoundtripLatency(int v) = 0;
			virtual const std::vector<float>& getStimulus() const = 0;
			virtual const Response& getResponse() const = 0;
			virtual void clearResponse() = 0;
			static Ptr createInstance(juce::AudioDeviceManager* adm);
		};

	} // namespace IR

} // namespace BAAPP
