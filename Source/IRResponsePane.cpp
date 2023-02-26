//
//  IRResponsePane.cpp
//  BodeAnalyzer_App
//
//  created by yu2924 on 2023-02-06
//

#include "IRResponsePane.h"
#include "MagPhasePlotPane.h"
#include "ProgressiveSessionBase.h"
#include "SeqNum.h"

namespace BAAPP
{
	namespace IR
	{

		class CDArrayDataSourceBundle : public MagPhasePlotPane::DataSourceBundle
		{
		public:
			using CD = std::complex<double>;
			class CDArrayDataSource : public Plot2dPane::DataSource
			{
			public:
				double sampleRate = 0;
				const std::vector<CD>* data = nullptr;
				bool phaseAxis = false;
				CDArrayDataSource(int icolor, double fs, const std::vector<CD>* vs, bool ph) : sampleRate(fs), data(vs), phaseAxis(ph) { colorIndex = icolor; }
				virtual size_t size() const override { return data->size(); }
				virtual Plot2dPane::Point at(size_t i) const override
				{
					const CD& cd = data->at(i);
					size_t length = data->size();
					if(phaseAxis) return { (float)(sampleRate * i / length), (float)std::arg(cd) * 180 / juce::MathConstants<float>::pi };
					else return { (float)(sampleRate * i / length), 20 * log10(std::max(1e-6f, (float)std::abs(cd))) };
				}
			};
			CDArrayDataSourceBundle(int icolor, double fs, const std::vector<CD>* vs)
			{
				dataSourceA = new CDArrayDataSource(icolor, fs, vs, false);
				dataSourceP = new CDArrayDataSource(icolor, fs, vs, true);
			}
		};

		class ResponsePane::Impl : public ProgressiveSessionBase::Listener
		{
		public:
			ResponsePane& owner;
			AnalysisController::Ptr analysisController;
			juce::Label titleLabel;
			juce::TabbedComponent tabbedContainer;
			enum { Margin = 8, TitleHeight = 16 };
			Impl(ResponsePane& o, AnalysisController* ac)
				: owner(o)
				, analysisController(ac)
				, tabbedContainer(juce::TabbedButtonBar::TabsAtLeft)
			{
				owner.setOpaque(true);
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
				const AnalysisController::Response& response = analysisController->getResponse();
				double fs = response.sampleRate;
				if(fs <= 0) fs = 44100;
				int cch = (int)response.channelList.size();
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
				double fny = fs * 0.5;
				axisx.range = { 20, (float)fny };
				SNDecadeD sn = SNDecadeD::Floor(axisx.range.lval);
				if(sn.Mantissa() == 1) axisx.majTicks.push_back((float)sn);
				else				   axisx.minTicks.push_back((float)sn);
				for(sn = sn.Next(); sn < fny; sn = sn.Next())
				{
					if(sn.Mantissa() == 1) axisx.majTicks.push_back((float)sn);
					else				   axisx.minTicks.push_back((float)sn);
				}
				if(sn.Mantissa() == 1) axisx.majTicks.push_back((float)sn);
				else				   axisx.minTicks.push_back((float)sn);
				axisx.tickFlags = Plot2dPane::Both;
				axisx.gridFlags = Plot2dPane::Both;
				axisx.tickLabelFlags = Plot2dPane::Major;
				axisx.logscale = true;
				axisx.labelVisible = true;
				Plot2dPane::Axis axisa{};
				axisa.label = "Amplitude (dB)";
				axisa.range = { -100, 20 };
				axisa.majTicks = { -100, -80, -60, -40, -20, 0, 20 };
				axisa.minTicks = {};
				axisa.tickFlags = Plot2dPane::Both;
				axisa.gridFlags = Plot2dPane::Both;
				axisa.tickLabelFlags = Plot2dPane::Major;
				axisa.logscale = false;
				axisa.labelVisible = true;
				Plot2dPane::Axis axisp{};
				axisp.label = "Phase (deg.)";
				axisp.range = { -180, 180 };
				axisp.majTicks = { -180, -90, 0, 90, 180 };
				axisp.minTicks = {};
				axisp.tickFlags = Plot2dPane::Both;
				axisp.gridFlags = Plot2dPane::Both;
				axisp.tickLabelFlags = Plot2dPane::Major;
				axisp.logscale = false;
				axisp.labelVisible = true;
				for(int i = 0; i < ctabs; i++)
				{
					MagPhasePlotPane* pane = dynamic_cast<MagPhasePlotPane*>(tabbedContainer.getTabContentComponent(i));
					pane->setAxes(axisx, axisa, axisp);
					if(i < cch) pane->setDataSourceBundle(new CDArrayDataSourceBundle(1, fs, &response.channelList[i].FR));
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

		ResponsePane::ResponsePane(AnalysisController* ac) { impl = new Impl(*this, ac); }
		ResponsePane::~ResponsePane() { delete impl; }
		void ResponsePane::resized() { impl->resized(); }
		void ResponsePane::paint(juce::Graphics& g) { impl->paint(g); }

	} // namespace IR
} // namespace BAAPP
