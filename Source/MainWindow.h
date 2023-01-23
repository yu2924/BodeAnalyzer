//
//  MainWindow.h
//  BodeAnalyzer_App
//
//  created by yu2924 on 2023-02-05
//

#pragma once

#include <JuceHeader.h>
#include "AnalysisController.h"
#include "LatencyProbeController.h"

namespace BAAPP
{

	juce::Component* MainWindowCreateInstance(const juce::String& name, juce::AudioDeviceManager* adm, AnalysisController* ac, LatencyProbeController* lpc);

}
