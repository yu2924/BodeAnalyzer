//
//  LatencyProbeController.h
//  BodeAnalyzer_App
//
//  created by yu2924 on 2023-02-09
//

#pragma once

#include <JuceHeader.h>
#include "ProgressiveSessionBase.h"

namespace BAAPP
{

	class LatencyProbeController : public ProgressiveSessionBase, public juce::ChangeBroadcaster
	{
	protected:
		LatencyProbeController() {}
	public:
		using Ptr = juce::ReferenceCountedObjectPtr<LatencyProbeController>;
		virtual int getProbeLength() const = 0;
		virtual void setProbeLength(int v) = 0;
		virtual float getProbeAmplitude() const = 0;
		virtual void setProbeAmplitude(float v) = 0;
		virtual int getRoundtripLatency() const = 0;
		virtual void setRoundtripLatency(int v) = 0;
		static Ptr createInstance(juce::AudioDeviceManager* adm);
	};

} // namespace BAAPP
