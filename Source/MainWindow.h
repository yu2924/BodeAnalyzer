//
//  MainWindow.h
//  BodeAnalyzer_App
//
//  created by yu2924 on 2023-02-05
//

#pragma once

#include <JuceHeader.h>
#include "LatencyProbeController.h"
#include "IRAnalysisController.h"
#include "STAnalysisController.h"

namespace BAAPP
{

	class MainWindow : public juce::DocumentWindow, public juce::ChangeBroadcaster
	{
	protected:
		MainWindow(const juce::String& name, juce::Colour bgclr, int btns, bool addtodesktop) : DocumentWindow(name, bgclr, btns, addtodesktop) {}
	public:
		virtual int getActiveScheme() const = 0;
		virtual void setActiveScheme(int v, juce::NotificationType n) = 0;
		static MainWindow* createInstance(const juce::String& name, juce::AudioDeviceManager* adm, LatencyProbeController* lpc, IR::AnalysisController* irac, ST::AnalysisController* stac);
	};

}
