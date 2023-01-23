//
//  Application.cpp
//  BodeAnalyzer_App
//
//  created by yu2924 on 2023-02-07
//

#include "Application.h"
#include "MainWindow.h"

namespace BAAPP
{

	class BodeAnalyzerApplicationImpl : public BodeAnalyzerApplication, public juce::ChangeListener
	{
	private:
		std::unique_ptr<juce::Component> mainWindow;
	public:
		BodeAnalyzerApplicationImpl() {}
		virtual const juce::String getApplicationName() override { return ProjectInfo::projectName; }
		virtual const juce::String getApplicationVersion() override { return ProjectInfo::versionString; }
		virtual bool moreThanOneInstanceAllowed() override { return true; }
		virtual void initialise(const juce::String&) override
		{
			if(juce::LookAndFeel_V4* lf4 = reinterpret_cast<juce::LookAndFeel_V4*>(&juce::LookAndFeel::getDefaultLookAndFeel()))
			{
				lf4->setColourScheme(juce::LookAndFeel_V4::getLightColourScheme());
			}
			// propertiesFile
			juce::PropertiesFile::Options opt;
			opt.applicationName = ProjectInfo::projectName;
			opt.folderName = ProjectInfo::companyName;
			opt.filenameSuffix = "xml";
			opt.storageFormat = juce::PropertiesFile::StorageFormat::storeAsXML;
			propertiesFile = std::make_unique<juce::PropertiesFile>(opt);
			// audioDeviceManager
			audioDeviceManager = std::make_unique<juce::AudioDeviceManager>();
			std::unique_ptr<juce::XmlElement> audiosettings = propertiesFile->getXmlValue("AudioSettings");
			if(audiosettings) audioDeviceManager->initialise(1, 1, audiosettings.get(), true);
			else audioDeviceManager->initialiseWithDefaultDevices(1, 1);
			// analysisController
			analysisController = AnalysisController::createInstance(audioDeviceManager.get());
			AnalysisController::Parameters params = {};
			params.method = (AnalysisController::Method)propertiesFile->getIntValue("Method", AnalysisController::Method::MethodLogSS);
			params.order = propertiesFile->getIntValue("Order", 16);
			params.amplitude = (float)propertiesFile->getDoubleValue("Amplitude", 0.5f);
			params.repeatCount = propertiesFile->getIntValue("RepeatCount", 1);
			analysisController->setParameters(params);
			analysisController->setRoundTripLatency(propertiesFile->getIntValue("RoundTripLatency", 0));
			// latencyProbeController
			latencyProbeController = LatencyProbeController::createInstance(audioDeviceManager.get());
			latencyProbeController->setProbeAmplitude((float)propertiesFile->getDoubleValue("ProbeAmplitude", 0.5));
			latencyProbeController->setProbeLength(propertiesFile->getIntValue("ProbeLength", 4096));
			// mainWindow
			mainWindow.reset(MainWindowCreateInstance(getApplicationName(), audioDeviceManager.get(), analysisController.get(), latencyProbeController.get()));
			// startup
			audioDeviceManager->addChangeListener(this);
			analysisController->addChangeListener(this);
			latencyProbeController->addChangeListener(this);
		}
		virtual void shutdown() override
		{
			latencyProbeController->removeChangeListener(this);
			analysisController->removeChangeListener(this);
			audioDeviceManager->removeChangeListener(this);
			mainWindow = nullptr;
		}
		virtual void systemRequestedQuit() override
		{
			quit();
		}
		virtual void anotherInstanceStarted(const juce::String&) override
		{
		}
		virtual void changeListenerCallback(juce::ChangeBroadcaster* source) override
		{
			if(source == audioDeviceManager.get())
			{
				std::unique_ptr<juce::XmlElement> audiosettings = audioDeviceManager->createStateXml();
				if(audiosettings) propertiesFile->setValue("AudioSettings", audiosettings.get());
			}
			else if(source == analysisController.get())
			{
				const AnalysisController::Parameters& params = analysisController->getParameters();
				propertiesFile->setValue("Method", params.method);
				propertiesFile->setValue("Order", params.order);
				propertiesFile->setValue("Amplitude", params.amplitude);
				propertiesFile->setValue("RepeatCount", params.repeatCount);
				propertiesFile->setValue("RoundTripLatency", analysisController->getRoundTripLatency());
			}
			else if(source == latencyProbeController.get())
			{
				propertiesFile->setValue("ProbeAmplitude", latencyProbeController->getProbeAmplitude());
				propertiesFile->setValue("ProbeLength", latencyProbeController->getProbeLength());
			}
		}
	};

	BodeAnalyzerApplication* TheApp()
	{
		return dynamic_cast<BodeAnalyzerApplication*>(juce::JUCEApplication::getInstance());
	}

} // namespace BAAPP

// the entry point
START_JUCE_APPLICATION(BAAPP::BodeAnalyzerApplicationImpl)
