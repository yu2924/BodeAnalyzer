//
//  Application.h
//  BodeAnalyzer_App
//
//  created by yu2924 on 2023-02-07
//

#pragma once

#include <JuceHeader.h>
#include "AnalysisController.h"
#include "LatencyProbeController.h"

namespace BAAPP
{

	class BodeAnalyzerApplication : public juce::JUCEApplication
	{
	public:
		std::unique_ptr<juce::PropertiesFile> propertiesFile;
		std::unique_ptr < juce::AudioDeviceManager> audioDeviceManager;
		AnalysisController::Ptr analysisController;
		LatencyProbeController::Ptr latencyProbeController;
	protected:
		BodeAnalyzerApplication() {}
	};

	BodeAnalyzerApplication* TheApp();

} // namespace BAAPP