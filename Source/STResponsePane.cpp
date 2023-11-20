//
//  STResponsePane.cpp
//  BodeAnalyzer_App
//
//  created by yu2924 on 2023-02-22
//

#include "STResponsePane.h"
#include "MagPhasePlotPane.h"
#include "ProgressiveSessionBase.h"
#include "SeqNum.h"
#include "FloatGrain.h"

namespace BAAPP
{
	namespace ST
	{

		class MPArrayDataSourceBundle : public MagPhasePlotPane::DataSourceBundle
		{
		public:
			using MP = AnalysisController::MeasuringPoint;
			class MPArrayDataSource : public Plot2dPane::DataSource
			{
			public:
				const std::vector<MP>* data = nullptr;
				bool phaseAxis = false;
				MPArrayDataSource(int icolor, const std::vector<MP>* vs, bool ph) : data(vs), phaseAxis(ph) { colorIndex = icolor; }
				virtual size_t size() const override { return data->size(); }
				virtual Plot2dPane::Point at(size_t i) const override
				{
					const MP& mp = data->at(i);
					if(phaseAxis) return { (float)mp.frequencyHz, mp.phaseDeg };
					else return { (float)mp.frequencyHz, 20 * std::log10(std::max(1e-6f, mp.amplitude)) };
				}
			};
			MPArrayDataSourceBundle(int icolor, const std::vector<MP>* vs)
			{
				dataSourceA = new MPArrayDataSource(icolor, vs, false);
				dataSourceP = new MPArrayDataSource(icolor, vs, true);
			}
		};

		class ResponsePane::Impl : public juce::Timer, public ProgressiveSessionBase::Listener
		{
		public:
			ResponsePane& owner;
			AnalysisController::Ptr analysisController;
			juce::Label titleLabel;
			juce::TextButton exportButton;
			juce::TabbedComponent tabbedContainer;
			enum { Margin = 8, TitleHeight = 24, ButtonWidth = 96 };
			Impl(ResponsePane& o, AnalysisController* ac)
				: owner(o)
				, analysisController(ac)
				, tabbedContainer(juce::TabbedButtonBar::TabsAtLeft)
			{
				owner.setOpaque(true);
				owner.addAndMakeVisible(titleLabel);
				titleLabel.setFont(juce::Font(15.00f, juce::Font::plain).withTypefaceStyle("Bold"));
				titleLabel.setText("Response", juce::dontSendNotification);
				owner.addAndMakeVisible(exportButton);
				exportButton.setButtonText("Export...");
				exportButton.onClick = [this]() { onClickExport(); };
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
				const AnalysisController::Parameters& params = analysisController->getParameters();
				Plot2dPane::Axis axisx{};
				axisx.label = "Frequency (Hz)";
				if(params.logarithmic)
				{
					double fmax = std::min(fs * 0.5, params.freqMaxHz);
					SNDecadeD sn = SNDecadeD::Floor(params.freqMinHz);
					axisx.range.lval = (float)sn;
					if(sn.Mantissa() == 1) axisx.majTicks.push_back((float)sn);
					else				   axisx.minTicks.push_back((float)sn);
					for(sn = sn.Next(); sn < fmax; sn = sn.Next())
					{
						if(sn.Mantissa() == 1) axisx.majTicks.push_back((float)sn);
						else				   axisx.minTicks.push_back((float)sn);
					}
					if(sn.Mantissa() == 1) axisx.majTicks.push_back((float)sn);
					else				   axisx.minTicks.push_back((float)sn);
					axisx.range.hval = (float)sn;
				}
				else
				{
					double fmax = std::min(fs * 0.5, params.freqMaxHz);
					double fdiv = SN125D::Ceil((params.freqMaxHz - params.freqMinHz) * 0.1);
					double f = FloatGrain::truncate(params.freqMinHz, fdiv);
					axisx.majTicks.push_back((float)f);
					for(f += fdiv; f < fmax; f += fdiv) axisx.majTicks.push_back((float)f);
					axisx.majTicks.push_back((float)f);
					axisx.range = { axisx.majTicks.front(), axisx.majTicks.back() };
					axisx.minTicks = {};
				}
				axisx.tickFlags = Plot2dPane::Both;
				axisx.gridFlags = Plot2dPane::Both;
				axisx.tickLabelFlags = Plot2dPane::Major;
				axisx.logscale = params.logarithmic;
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
					if(i < cch) pane->setDataSourceBundle(new MPArrayDataSourceBundle(1, &response.channelList[i].pointList));
				}
			}
			void onClickExport()
			{
				std::shared_ptr<juce::FileChooser> pfc = std::make_shared<juce::FileChooser>("Export", juce::File(), "*.csv");
				pfc->launchAsync(juce::FileBrowserComponent::FileChooserFlags::saveMode | juce::FileBrowserComponent::FileChooserFlags::warnAboutOverwriting, [this, pfc](const juce::FileChooser& fc)
				{
					juce::File path = fc.getResult();
					if(path == juce::File()) return;
					const auto& resp = analysisController->getResponse();
					int cch = (int)resp.channelList.size();
					if(cch <= 0) return;
					size_t len = resp.channelList.front().pointList.size();
					std::unique_ptr<juce::FileOutputStream> str = std::make_unique<juce::FileOutputStream>(path);
					if(str->failedToOpen()) return;
					str->setPosition(0);
					str->truncate();
					juce::StringArray sa;
					sa.ensureStorageAllocated(1 + 2 * cch);
					sa.add("frequencyHz");
					for(int ich = 0; ich < cch; ++ich)
					{
						sa.add(juce::String::formatted("response[%d].amplitude", ich));
						sa.add(juce::String::formatted("response[%d].phaseDeg", ich));
					}
					juce::String hdr = sa.joinIntoString(",") + "\r\n";
					str->write(hdr.toRawUTF8(), hdr.getNumBytesAsUTF8());
					for(size_t i = 0; i < len; ++i)
					{
						sa.clearQuick();
						sa.add(juce::String(resp.channelList[0].pointList[i].frequencyHz));
						for(int ich = 0; ich < cch; ++ich)
						{
							auto mp = resp.channelList[0].pointList[i];
							sa.add(juce::String(mp.amplitude));
							sa.add(juce::String(mp.phaseDeg));
						}
						juce::String line = sa.joinIntoString(",") + "\r\n";
						str->write(line.toRawUTF8(), line.getNumBytesAsUTF8());
					}
				});
			}
			// --------------------------------------------------------------------------------
			// juce::Component
			void resized()
			{
				juce::Rectangle<int> rc = owner.getLocalBounds().reduced(Margin);
				juce::Rectangle<int> rchdr = rc.removeFromTop(TitleHeight);
				exportButton.setBounds(rchdr.removeFromRight(ButtonWidth));
				titleLabel.setBounds(rchdr);
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
			// juce::Timer
			virtual void timerCallback() override
			{
				MagPhasePlotPane* pane = dynamic_cast<MagPhasePlotPane*>(tabbedContainer.getCurrentContentComponent());
				pane->repaint();
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
					juce::MessageManager::callAsync([this]()
					{
						refillContents();
						startTimer(500);
					});
				}
			}
			virtual void progressiveSessionEnd(ProgressiveSessionBase* ps, const juce::Result&, bool) override
			{
				if(ps == analysisController.get())
				{
					stopTimer();
					MagPhasePlotPane* pane = dynamic_cast<MagPhasePlotPane*>(tabbedContainer.getCurrentContentComponent());
					pane->repaint();
				}
			}
		};

		ResponsePane::ResponsePane(AnalysisController* ac) { impl = new Impl(*this, ac); }
		ResponsePane::~ResponsePane() { delete impl; }
		void ResponsePane::resized() { impl->resized(); }
		void ResponsePane::paint(juce::Graphics& g) { impl->paint(g); }

	} // namespace ST
} // namespace BAAPP
