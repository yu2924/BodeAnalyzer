//
//  ResponsePane.cpp
//  BodeAnalyzer_App
//
//  created by yu2924 on 2023-02-06
//

#include "ResponsePane.h"
#include "MagPhasePlotPane.h"

namespace BAAPP
{

	class ResponsePane::Impl : public ProgressiveSessionBase::Listener
	{
	public:
		ResponsePane& owner;
		juce::AudioDeviceManager* audioDeviceManager;
		AnalysisController::Ptr analysisController;
		juce::Label titleLabel;
		juce::TabbedComponent tabbedContainer;
		enum { Margin = 8, TitleHeight = 16 };
		Impl(ResponsePane& o, juce::AudioDeviceManager* adm, AnalysisController* ac)
			: owner(o)
			, audioDeviceManager(adm)
			, analysisController(ac)
			, tabbedContainer(juce::TabbedButtonBar::TabsAtLeft)
		{
			owner.addAndMakeVisible(titleLabel);
			titleLabel.setFont(juce::Font(15.00f, juce::Font::plain).withTypefaceStyle("Bold"));
			titleLabel.setText("Response", juce::dontSendNotification);
			owner.addAndMakeVisible(tabbedContainer);
			MagPhasePlotPane* pane = new MagPhasePlotPane;
			tabbedContainer.addTab("1", juce::Colours::white, pane, true, 0);
			refillContents();
			analysisController->addProgressiveSessionListener(this);
		}
		~Impl()
		{
			analysisController->removeProgressiveSessionListener(this);
		}
		// --------------------------------------------------------------------------------
		// internal
		void refillContents()
		{
			double fs = analysisController->getCurrentSampleRate();
			const AnalysisController::Parameters& params = analysisController->getParameters();
			int length = 1 << params.order;
			const std::vector<AnalysisController::Response>& responses = analysisController->getResponseList();
			int cch = (int)responses.size();
			int ctabs = std::max(1, cch);
			while(ctabs < tabbedContainer.getNumTabs())
			{
				tabbedContainer.removeTab(tabbedContainer.getNumTabs() - 1);
			}
			while(tabbedContainer.getNumTabs() < ctabs)
			{
				int i = tabbedContainer.getNumTabs();
				MagPhasePlotPane* pane = new MagPhasePlotPane;
				tabbedContainer.addTab(juce::String(i + 1), juce::Colours::white, pane, true, i);
			}
			Plot2dPane::Axis axisx{};
			axisx.label = "Frequency (Hz)";
			axisx.range = { 10, (float)fs * 0.5f };
			for(double f = 10; f <= fs; f *= 10) axisx.majTicks.push_back((float)f);
			if(std::find(axisx.majTicks.begin(), axisx.majTicks.end(), axisx.range.hval) == axisx.majTicks.end()) axisx.majTicks.push_back(axisx.range.hval);
			axisx.minTicks = {};
			axisx.tickFlags = Plot2dPane::Both;
			axisx.gridFlags = Plot2dPane::Major;
			axisx.tickLabelFlags = Plot2dPane::Major;
			axisx.logscale = true;
			axisx.labelVisible = true;
			Plot2dPane::Axis axisa{};
			axisa.label = "Amplitude (dB)";
			axisa.range = { -100, 20 };
			axisa.majTicks = { -100, -80, -60, -40, -20, 0, 20 };
			axisa.minTicks = {};
			axisa.tickFlags = Plot2dPane::Major;
			axisa.gridFlags = Plot2dPane::Major;
			axisa.tickLabelFlags = Plot2dPane::Major;
			axisa.logscale = false;
			axisa.labelVisible = true;
			Plot2dPane::Axis axisp{};
			axisp.label = "Phase (deg.)";
			axisp.range = { -180, 180 };
			axisp.majTicks = { -180, -90, 0, 90, 180 };
			axisp.minTicks = {};
			axisp.tickFlags = Plot2dPane::Major;
			axisp.gridFlags = Plot2dPane::Major;
			axisp.tickLabelFlags = Plot2dPane::Major;
			axisp.logscale = false;
			axisp.labelVisible = true;
			for(int i = 0; i < ctabs; i ++)
			{
				MagPhasePlotPane* pane = dynamic_cast<MagPhasePlotPane*>(tabbedContainer.getTabContentComponent(i));
				pane->setAxes(axisx, axisa, axisp);
				if(i < cch)
				{
					const AnalysisController::Response& resp = responses[i];
					auto dsb = MagPhasePlotPane::createDataSourceBundle(1, resp.FR.data(), resp.FR.size(), false,
						[fs, length](size_t i, std::complex<double> v)->Plot2dPane::Point { double vs = std::max(1e-6, std::abs(v)); return { (float)(fs * i / length), 20 * (float)log10(vs) }; },
						[fs, length](size_t i, std::complex<double> v)->Plot2dPane::Point { return { (float)(fs * i / length), (float)std::arg(v) * 180 / juce::float_Pi }; });
					pane->setDataSourceBundle(dsb);
				}
			}
		}
		// --------------------------------------------------------------------------------
		// juce::Component
		void resized()
		{
			juce::Rectangle<int> rc = owner.getLocalBounds().reduced(Margin);
			titleLabel.setBounds(rc.removeFromTop(TitleHeight));
			tabbedContainer.setBounds(rc);
		}
		void paint(juce::Graphics& g)
		{
			g.fillAll(juce::Colours::white);
			juce::Rectangle<float> rc = owner.getLocalBounds().toFloat().reduced(4);
			g.setColour(juce::Colours::lightgrey);
			g.drawRoundedRectangle(rc, 3, 2);
		}
		// --------------------------------------------------------------------------------
		// AnalysisController::Listener
		virtual void progressiveSessionBegin(ProgressiveSessionBase* ps) override
		{
			if(ps == analysisController.get())
			{
				for(int c = tabbedContainer.getNumTabs(), i = 0; i < c; ++i)
				{
					MagPhasePlotPane* pane = dynamic_cast<MagPhasePlotPane*>(tabbedContainer.getTabContentComponent(i));
					pane->clearDataSourceBundle();
				}
			}
		}
		virtual void progressiveSessionEnd(ProgressiveSessionBase* ps, const juce::Result& result, bool aborted) override
		{
			if(ps == analysisController.get())
			{
				if(result.wasOk() && !aborted)
				{
					refillContents();
				}
			}
		}
	};

	ResponsePane::ResponsePane(juce::AudioDeviceManager* adm, AnalysisController* ac) { impl = new Impl(*this, adm, ac); }
	ResponsePane::~ResponsePane() { delete impl; }
	void ResponsePane::resized() { impl->resized(); }
	void ResponsePane::paint(juce::Graphics& g) { impl->paint(g); }

} // namespace BAAPP
