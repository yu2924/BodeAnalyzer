//
//  STAnalysisController.h
//  BodeAnalyzer_App
//
//  created by yu2924 on 2023-02-22
//

#pragma once

#include "ProgressiveSessionBase.h"
#include <vector>

namespace BAAPP
{
	namespace ST
	{

		class AnalysisController : public ProgressiveSessionBase, public juce::ChangeBroadcaster
		{
		protected:
			AnalysisController() {}
		public:
			using Ptr = juce::ReferenceCountedObjectPtr<AnalysisController>;
			struct Parameters
			{
				double freqMinHz;
				double freqMaxHz;
				int numPoints;
				float amplitude;
				int repeatCount;
				bool logarithmic;
			};
			struct MeasuringPoint
			{
				double frequencyHz;
				float amplitude;
				float phaseDeg;
			};
			struct Response
			{
				double sampleRate;
				struct Channel
				{
					std::vector<MeasuringPoint> pointList;
				};
				std::vector<Channel> channelList;
			};
			virtual const Parameters& getParameters() const = 0;
			virtual void setParameters(const Parameters& v) = 0;
			virtual int getRoundtripLatency() const = 0;
			virtual void setRoundtripLatency(int v) = 0;
			virtual const Response& getResponse() const = 0;
			virtual void clearResponse() = 0;
			static Ptr createInstance(juce::AudioDeviceManager* adm);
		};

	} // namespace ST
} // namespace BAAPP
