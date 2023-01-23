//
//  MainWindow.cpp
//  BodeAnalyzer_App
//
//  created by yu2924 on 2023-02-05
//

#include "MainWindow.h"
#include "Application.h"
#include "AnalysisController.h"
#include "DeviceForm.h"
#include "StimulusForm.h"
#include "ResponsePane.h"

namespace BAAPP
{

	class MainPane : public juce::Component
	{
	private:
		std::unique_ptr<DeviceForm> deviceForm;
		std::unique_ptr<StimulusForm> stimulusForm;
		std::unique_ptr<ResponsePane> responsePane;
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainPane)
	public:
		MainPane(juce::AudioDeviceManager* adm, AnalysisController* ac, LatencyProbeController* lpc)
			: Component()
		{
			deviceForm.reset(new DeviceForm(adm, ac, lpc));
			stimulusForm.reset(new StimulusForm(adm, ac));
			responsePane.reset(new ResponsePane(adm, ac));
			addAndMakeVisible(deviceForm.get());
			addAndMakeVisible(stimulusForm.get());
			addAndMakeVisible(responsePane.get());
			setSize(800, 800);
		}
		virtual ~MainPane()
		{
		}
		virtual void resized() override
		{
			juce::Rectangle<int> rc = getLocalBounds();
			deviceForm->setBounds(rc.removeFromTop(deviceForm->getHeight()));
			stimulusForm->setBounds(rc.removeFromTop(stimulusForm->getHeight()));
			responsePane->setBounds(rc);
		}
		virtual void paint(juce::Graphics& g) override
		{
			g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
		}
	};

	class MainWindow : public juce::DocumentWindow
	{
	private:
		juce::TooltipWindow tooltipWindow;
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
	public:
		MainWindow(juce::String name, juce::AudioDeviceManager* adm, AnalysisController* ac, LatencyProbeController* lpc)
			: DocumentWindow(name, juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId), DocumentWindow::allButtons)
		{
			setUsingNativeTitleBar(true);
			setContentOwned(new MainPane(adm, ac, lpc), true);
			setResizable(true, true);
			centreWithSize(getWidth(), getHeight());
			setVisible(true);
		}
		virtual void closeButtonPressed() override
		{
			juce::JUCEApplication::getInstance()->systemRequestedQuit();
		}
	};

	juce::Component* MainWindowCreateInstance(const juce::String& name, juce::AudioDeviceManager* adm, AnalysisController* ac, LatencyProbeController* lpc)
	{
		return new MainWindow(name, adm, ac, lpc);
	}

} // namespace BAAPP