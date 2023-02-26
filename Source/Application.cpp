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
		std::unique_ptr<MainWindow> mainWindow;
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
			// latencyProbeController
			latencyProbeController = LatencyProbeController::createInstance(audioDeviceManager.get());
			latencyProbeController->setProbeAmplitude((float)propertiesFile->getDoubleValue("ProbeAmplitude", 0.5));
			latencyProbeController->setProbeLength(propertiesFile->getIntValue("ProbeLength", 4096));
			int roundtriplatency = propertiesFile->getIntValue("RoundtripLatency", 0);
			latencyProbeController->setRoundtripLatency(roundtriplatency);
			// irAnalysisController
			irAnalysisController = IR::AnalysisController::createInstance(audioDeviceManager.get());
			IR::AnalysisController::Parameters fparams = {};
			fparams.method = (IR::AnalysisController::Method)propertiesFile->getIntValue("IR-Method", IR::AnalysisController::Method::MethodLogSS);
			fparams.order = propertiesFile->getIntValue("IR-Order", 16);
			fparams.amplitude = (float)propertiesFile->getDoubleValue("IR-Amplitude", 0.5);
			fparams.repeatCount = propertiesFile->getIntValue("IR-RepeatCount", 1);
			irAnalysisController->setParameters(fparams);
			irAnalysisController->setRoundtripLatency(roundtriplatency);
			// stAnalysisController
			stAnalysisController = ST::AnalysisController::createInstance(audioDeviceManager.get());
			ST::AnalysisController::Parameters sparams = {};
			sparams.freqMinHz = propertiesFile->getDoubleValue("ST-FreqMin", 20);
			sparams.freqMaxHz = propertiesFile->getDoubleValue("ST-FreqMax", 20000);
			sparams.numPoints = propertiesFile->getIntValue("ST-NumPoints", 88);
			sparams.amplitude = (float)propertiesFile->getDoubleValue("ST-Amplitude", 0.5);
			sparams.repeatCount = propertiesFile->getIntValue("ST-RepeatCount", 1);
			sparams.logarithmic = propertiesFile->getBoolValue("ST-Logarithmic", true);
			stAnalysisController->setParameters(sparams);
			stAnalysisController->setRoundtripLatency(roundtriplatency);
			// mainWindow
			mainWindow.reset(MainWindow::createInstance(getApplicationName(), audioDeviceManager.get(), latencyProbeController.get(), irAnalysisController.get(), stAnalysisController.get()));
			mainWindow->setActiveScheme(propertiesFile->getIntValue("ActiveScheme", 0), juce::dontSendNotification);
			// startup
			audioDeviceManager->addChangeListener(this);
			latencyProbeController->addChangeListener(this);
			irAnalysisController->addChangeListener(this);
			stAnalysisController->addChangeListener(this);
			mainWindow->addChangeListener(this);
		}
		virtual void shutdown() override
		{
			mainWindow->removeChangeListener(this);
			stAnalysisController->removeChangeListener(this);
			irAnalysisController->removeChangeListener(this);
			latencyProbeController->removeChangeListener(this);
			audioDeviceManager->removeChangeListener(this);
			propertiesFile->saveIfNeeded();
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
			else if(source == latencyProbeController.get())
			{
				propertiesFile->setValue("ProbeAmplitude", latencyProbeController->getProbeAmplitude());
				propertiesFile->setValue("ProbeLength", latencyProbeController->getProbeLength());
				int roundtriplatency = latencyProbeController->getRoundtripLatency();
				propertiesFile->setValue("RoundtripLatency", roundtriplatency);
				irAnalysisController->setRoundtripLatency(roundtriplatency);
				stAnalysisController->setRoundtripLatency(roundtriplatency);
			}
			else if(source == irAnalysisController.get())
			{
				const IR::AnalysisController::Parameters& params = irAnalysisController->getParameters();
				propertiesFile->setValue("IR-Method", params.method);
				propertiesFile->setValue("IR-Order", params.order);
				propertiesFile->setValue("IR-Amplitude", params.amplitude);
				propertiesFile->setValue("IR-RepeatCount", params.repeatCount);
			}
			else if(source == stAnalysisController.get())
			{
				const ST::AnalysisController::Parameters& params = stAnalysisController->getParameters();
				propertiesFile->setValue("ST-FreqMin", params.freqMinHz);
				propertiesFile->setValue("ST-FreqMax", params.freqMaxHz);
				propertiesFile->setValue("ST-NumPoints", params.numPoints);
				propertiesFile->setValue("ST-Amplitude", params.amplitude);
				propertiesFile->setValue("ST-RepeatCount", params.repeatCount);
				propertiesFile->setValue("ST-Logarithmic", params.logarithmic);
			}
			else if(source == mainWindow.get())
			{
				propertiesFile->setValue("ActiveScheme", mainWindow->getActiveScheme());
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
