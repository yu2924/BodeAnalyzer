//
//  Application.h
//  BodeAnalyzer_App
//
//  created by yu2924 on 2023-02-07
//

#pragma once

#include <JuceHeader.h>
#include "LatencyProbeController.h"
#include "IRAnalysisController.h"
#include "STAnalysisController.h"

namespace BAAPP
{

	class BodeAnalyzerApplication : public juce::JUCEApplication
	{
	public:
		std::unique_ptr<juce::PropertiesFile> propertiesFile;
		std::unique_ptr < juce::AudioDeviceManager> audioDeviceManager;
		LatencyProbeController::Ptr latencyProbeController;
		IR::AnalysisController::Ptr irAnalysisController;
		ST::AnalysisController::Ptr stAnalysisController;
	protected:
		BodeAnalyzerApplication() {}
	};

	BodeAnalyzerApplication* TheApp();

} // namespace BAAPP