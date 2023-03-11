//
//  MainWindow.cpp
//  BodeAnalyzer_App
//
//  created by yu2924 on 2023-02-05
//

#include "MainWindow.h"
#include "Application.h"
#include "DeviceForm.h"
#include "IRStimulusForm.h"
#include "IRResponsePane.h"
#include "STStimulusForm.h"
#include "STResponsePane.h"

namespace BAAPP
{
		class IRPane : public juce::Component
		{
		private:
			std::unique_ptr<IR::StimulusForm> stimulusForm;
			std::unique_ptr<IR::ResponsePane> responsePane;
		public:
			IRPane(juce::AudioDeviceManager* adm, IR::AnalysisController* ac)
			{
				setOpaque(true);
				stimulusForm.reset(new IR::StimulusForm(adm, ac));
				responsePane.reset(new IR::ResponsePane(ac));
				addAndMakeVisible(stimulusForm.get());
				addAndMakeVisible(responsePane.get());
			}
			virtual void resized() override
			{
				juce::Rectangle<int> rc = getLocalBounds();
				stimulusForm->setBounds(rc.removeFromTop(stimulusForm->getHeight()));
				responsePane->setBounds(rc);
			}
			virtual void paint(juce::Graphics& g) override
			{
				g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
			}
		};

		class STPane : public juce::Component
		{
		private:
			std::unique_ptr<ST::StimulusForm> stimulusForm;
			std::unique_ptr<ST::ResponsePane> responsePane;
		public:
			STPane(juce::AudioDeviceManager* adm, ST::AnalysisController* ac)
			{
				setOpaque(true);
				stimulusForm.reset(new ST::StimulusForm(adm, ac));
				responsePane.reset(new ST::ResponsePane(ac));
				addAndMakeVisible(stimulusForm.get());
				addAndMakeVisible(responsePane.get());
			}
			virtual void resized() override
			{
				juce::Rectangle<int> rc = getLocalBounds();
				stimulusForm->setBounds(rc.removeFromTop(stimulusForm->getHeight()));
				responsePane->setBounds(rc);
			}
			virtual void paint(juce::Graphics& g) override
			{
				g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
			}
		};

	class MainPane : public juce::Component
	{
	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainPane)
	public:
		std::unique_ptr<DeviceForm> deviceForm;
		std::unique_ptr<juce::TabbedComponent> tabbedContainer;
		std::unique_ptr<IRPane> irPane;
		std::unique_ptr<STPane> stPane;
		MainPane(juce::AudioDeviceManager* adm, LatencyProbeController* lpc, IR::AnalysisController* irac, ST::AnalysisController* stac)
			: Component()
		{
			setOpaque(true);
			deviceForm.reset(new DeviceForm(adm, lpc));
			addAndMakeVisible(deviceForm.get());
			tabbedContainer.reset(new juce::TabbedComponent(juce::TabbedButtonBar::Orientation::TabsAtTop));
			irPane.reset(new IRPane(adm, irac));
			stPane.reset(new STPane(adm, stac));
			tabbedContainer->addTab("IR", juce::Colours::white, irPane.get(), false, -1);
			tabbedContainer->addTab("Stepped Sweep", juce::Colours::white, stPane.get(), false, -1);
			addAndMakeVisible(tabbedContainer.get());
			setSize(800, 800);
		}
		virtual ~MainPane()
		{
		}
		virtual void resized() override
		{
			juce::Rectangle<int> rc = getLocalBounds();
			deviceForm->setBounds(rc.removeFromTop(deviceForm->getHeight()));
			tabbedContainer->setBounds(rc);
		}
		virtual void paint(juce::Graphics& g) override
		{
			g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
		}
	};

	class MainWindowImpl : public MainWindow, public juce::ChangeListener
	{
	private:
		juce::TooltipWindow tooltipWindow;
		std::unique_ptr<MainPane> mainPane;
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindowImpl)
	public:
		MainWindowImpl(juce::String name, juce::AudioDeviceManager* adm, LatencyProbeController* lpc, IR::AnalysisController* irac, ST::AnalysisController* stac)
			: MainWindow(name, juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId), DocumentWindow::allButtons, true)
		{
			setUsingNativeTitleBar(true);
			mainPane.reset(new MainPane(adm, lpc, irac, stac));
			setContentNonOwned(mainPane.get(), true);
			setResizable(true, true);
			centreWithSize(getWidth(), getHeight());
			setVisible(true);
			mainPane->tabbedContainer->getTabbedButtonBar().addChangeListener(this);
		}
		virtual ~MainWindowImpl()
		{
			mainPane->tabbedContainer->getTabbedButtonBar().removeChangeListener(this);
			clearContentComponent();
			mainPane = nullptr;
		}
		virtual void closeButtonPressed() override
		{
			juce::JUCEApplication::getInstance()->systemRequestedQuit();
		}
		virtual int getActiveScheme() const override
		{
			return mainPane->tabbedContainer->getCurrentTabIndex();
		}
		virtual void setActiveScheme(int v, juce::NotificationType n) override
		{
			mainPane->tabbedContainer->setCurrentTabIndex(v, n);
		}
		virtual void changeListenerCallback(juce::ChangeBroadcaster* source) override
		{
			if(source == &mainPane->tabbedContainer->getTabbedButtonBar())
			{
				sendChangeMessage();
			}
		}
	};

	MainWindow* MainWindow::createInstance(const juce::String& name, juce::AudioDeviceManager* adm, LatencyProbeController* lpc, IR::AnalysisController* irac, ST::AnalysisController* stac)
	{
		return new MainWindowImpl(name, adm, lpc, irac, stac);
	}

} // namespace BAAPP