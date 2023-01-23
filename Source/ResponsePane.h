//
//  ResponsePane.h
//  BodeAnalyzer_App
//
//  created by yu2924 on 2023-02-06
//

#pragma once

#include <JuceHeader.h>
#include "AnalysisController.h"

namespace BAAPP
{

	class ResponsePane : public juce::Component
	{
	protected:
		class Impl;
		Impl* impl;
	public:
		ResponsePane(juce::AudioDeviceManager* adm, AnalysisController* ac);
		virtual ~ResponsePane();
		virtual void resized() override;
		virtual void paint(juce::Graphics& g) override;
	};

} // namespace BAAPP
