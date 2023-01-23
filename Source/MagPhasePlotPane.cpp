//
//  MagPhasePlotPane.cpp
//  simple 2D plot library
//
//  created by yu2924 on 2023-01-26
//

#include "MagPhasePlotPane.h"
#include <complex>

MagPhasePlotPane::MagPhasePlotPane()
{
	setOpaque(true);
	addAndMakeVisible(plotPaneA);
	addAndMakeVisible(plotPaneP);
}

void MagPhasePlotPane::resized()
{
	juce::Rectangle<int> rc = getLocalBounds();
	int cy = rc.getHeight() / 2;
	plotPaneA.setBounds(rc.removeFromTop(cy));
	plotPaneP.setBounds(rc.removeFromTop(cy));
}

void MagPhasePlotPane::setAxes(const Plot2dPane::Axis& axisx, const Plot2dPane::Axis& axisa, const Plot2dPane::Axis& axisp)
{
	Plot2dPane::Axis axisx_ = axisx;
	axisx_.tickFlags = Plot2dPane::None;
	axisx_.tickLabelFlags = Plot2dPane::None;
	axisx_.labelVisible = false;
	plotPaneA.setAxisX(axisx_);
	axisx_.tickFlags = axisx.tickFlags;
	axisx_.tickLabelFlags = axisx.tickLabelFlags;
	axisx_.labelVisible = axisx.labelVisible;
	plotPaneP.setAxisX(axisx_);
	plotPaneA.setAxisY(axisa);
	plotPaneP.setAxisY(axisp);
	plotPaneA.adjustLabelBorder();
	plotPaneP.adjustLabelBorder();
	juce::BorderSize<int> ba = plotPaneA.getLabelBorder();
	juce::BorderSize<int> bp = plotPaneP.getLabelBorder();
	int cxleft = std::max(ba.getLeft(), bp.getLeft());
	ba.setLeft(cxleft);
	bp.setLeft(cxleft);
	plotPaneA.setLabelBorder(ba);
	plotPaneP.setLabelBorder(bp);
}

void MagPhasePlotPane::setAxes(const juce::String& xlabel, Plot2dPane::Range xrange, const std::vector<float>& xticks, bool xlog, std::function<juce::String(float)> xTextFromValue,
							   const juce::String& alabel, Plot2dPane::Range arange, const std::vector<float>& aticks, bool alog, std::function<juce::String(float)> aTextFromValue,
							   const juce::String& plabel, Plot2dPane::Range prange, const std::vector<float>& pticks, bool plog, std::function<juce::String(float)> pTextFromValue)
{
	Plot2dPane::Axis axisx;
	axisx.label = xlabel;
	axisx.range = xrange;
	axisx.majTicks = xticks;
	axisx.minTicks = {};
	axisx.tickFlags = Plot2dPane::None; // set later
	axisx.gridFlags = Plot2dPane::Major;
	axisx.tickLabelFlags = Plot2dPane::Major; // set later
	axisx.logscale = xlog;
	axisx.labelVisible = false; // set later
	axisx.textFromValue = xTextFromValue;
	Plot2dPane::Axis axisa;
	axisa.label = alabel;
	axisa.range = arange;
	axisa.majTicks = aticks;
	axisa.minTicks = {};
	axisa.tickFlags = Plot2dPane::Both;
	axisa.gridFlags = Plot2dPane::Major;
	axisa.tickLabelFlags = Plot2dPane::Major;
	axisa.logscale = alog;
	axisa.labelVisible = alabel.isNotEmpty();
	axisa.textFromValue = aTextFromValue;
	Plot2dPane::Axis axisp;
	axisp.label = plabel;
	axisp.range = prange;
	axisp.majTicks = pticks;
	axisp.minTicks = {};
	axisp.tickFlags = Plot2dPane::Both;
	axisp.gridFlags = Plot2dPane::Major;
	axisp.tickLabelFlags = Plot2dPane::Major;
	axisp.logscale = plog;
	axisp.labelVisible = plabel.isNotEmpty();
	axisp.textFromValue = pTextFromValue;
	setAxes(axisx, axisa, axisp);
}

void MagPhasePlotPane::clearDataSourceBundle()
{
	dataSourceBundle = nullptr;
	plotPaneA.clearDataSources();
	plotPaneP.clearDataSources();
}

void MagPhasePlotPane::setDataSourceBundle(DataSourceBundle::Ptr dsb)
{
	dataSourceBundle = dsb;
	plotPaneA.setDataSource(dataSourceBundle ? dataSourceBundle->dataSourceA : nullptr);
	plotPaneP.setDataSource(dataSourceBundle ? dataSourceBundle->dataSourceP : nullptr);
}

// ================================================================================
// generic data source bundle builder

MagPhasePlotPane::DataSourceBundle::Ptr MagPhasePlotPane::createDataSourceBundle(int icolor, const std::complex<double>* pc, size_t len, bool copy,
	std::function<Plot2dPane::Point(size_t i, std::complex<double>)> magconv,
	std::function<Plot2dPane::Point(size_t i, std::complex<double>)> phconv)
{
	struct ComplexDataSourceBundle : public DataSourceBundle
	{
		Plot2dPane::DataRef<std::complex<double> > cdataref;
		std::function<Plot2dPane::Point(size_t i, std::complex<double>)> magconvert, phconvert;
		ComplexDataSourceBundle(int icolor, const std::complex<double>* pc, size_t len, bool copy,
			std::function<Plot2dPane::Point(size_t i, std::complex<double>)> magconv,
			std::function<Plot2dPane::Point(size_t i, std::complex<double>)> phconv)
			: cdataref(pc, len, copy), magconvert(magconv), phconvert(phconv)
		{
			class ADataSource : public Plot2dPane::DataSource
			{
			public:
				const ComplexDataSourceBundle& owner;
				ADataSource(const ComplexDataSourceBundle& bun, int icolor) : owner(bun) { colorIndex = icolor; }
				virtual size_t size() const override { return owner.cdataref.size(); }
				virtual Plot2dPane::Point at(size_t i) const override
				{
					if(owner.magconvert) return owner.magconvert(i, owner.cdataref[i]);
					else return { (float)i, (float)std::abs(owner.cdataref[i]) };
				}
			};
			class PDataSource : public Plot2dPane::DataSource
			{
			public:
				const ComplexDataSourceBundle& owner;
				PDataSource(const ComplexDataSourceBundle& bun, int icolor) : owner(bun) { colorIndex = icolor; }
				virtual size_t size() const override { return owner.cdataref.size(); }
				virtual Plot2dPane::Point at(size_t i) const override
				{
					if(owner.phconvert) return owner.phconvert(i, owner.cdataref[i]);
					else return { (float)i, (float)std::arg(owner.cdataref[i]) };
				}
			};
			dataSourceA = new ADataSource(*this, icolor);
			dataSourceP = new PDataSource(*this, icolor);
		}
	};
	return new ComplexDataSourceBundle(icolor, pc, len, copy, magconv, phconv);
}

MagPhasePlotPane::DataSourceBundle::Ptr MagPhasePlotPane::createDataSourceBundle(int icolor, const std::complex<float>* pc, size_t len, bool copy,
	std::function<Plot2dPane::Point(size_t i, std::complex<float>)> magconv,
	std::function<Plot2dPane::Point(size_t i, std::complex<float>)> phconv)
{
	struct ComplexDataSourceBundle : public DataSourceBundle
	{
		Plot2dPane::DataRef<std::complex<float> > cdataref;
		std::function<Plot2dPane::Point(size_t i, std::complex<float>)> magconvert, phconvert;
		ComplexDataSourceBundle(int icolor, const std::complex<float>* pc, size_t len, bool copy,
			std::function<Plot2dPane::Point(size_t i, std::complex<float>)> magconv,
			std::function<Plot2dPane::Point(size_t i, std::complex<float>)> phconv)
			: cdataref(pc, len, copy), magconvert(magconv), phconvert(phconv)
		{
			class ADataSource : public Plot2dPane::DataSource
			{
			public:
				const ComplexDataSourceBundle& owner;
				ADataSource(const ComplexDataSourceBundle& bun, int icolor) : owner(bun) { colorIndex = icolor; }
				virtual size_t size() const override { return owner.cdataref.size(); }
				virtual Plot2dPane::Point at(size_t i) const override
				{
					if(owner.magconvert) return owner.magconvert(i, owner.cdataref[i]);
					else return { (float)i, std::abs(owner.cdataref[i]) };
				}
			};
			class PDataSource : public Plot2dPane::DataSource
			{
			public:
				const ComplexDataSourceBundle& owner;
				PDataSource(const ComplexDataSourceBundle& bun, int icolor) : owner(bun) { colorIndex = icolor; }
				virtual size_t size() const override { return owner.cdataref.size(); }
				virtual Plot2dPane::Point at(size_t i) const override
				{
					if(owner.phconvert) return owner.phconvert(i, owner.cdataref[i]);
					else return { (float)i, std::arg(owner.cdataref[i]) };
				}
			};
			dataSourceA = new ADataSource(*this, icolor);
			dataSourceP = new PDataSource(*this, icolor);
		}
	};
	return new ComplexDataSourceBundle(icolor, pc, len, copy, magconv, phconv);
}

MagPhasePlotPane::DataSourceBundle::Ptr MagPhasePlotPane::createDataSourceBundle(int icolor, const float* pr, const float* pi, size_t len, bool copy,
	std::function<Plot2dPane::Point(size_t i, std::complex<float>)> magconv,
	std::function<Plot2dPane::Point(size_t i, std::complex<float>)> phconv)
{
	struct ComplexDataSourceBundle : public DataSourceBundle
	{
		Plot2dPane::DataRef<float> rdataref;
		Plot2dPane::DataRef<float> idataref;
		std::function<Plot2dPane::Point(size_t i, std::complex<float>)> magconvert, phconvert;
		ComplexDataSourceBundle(int icolor, const float* pr, const float* pi, size_t len, bool copy,
			std::function<Plot2dPane::Point(size_t i, std::complex<float>)> magconv,
			std::function<Plot2dPane::Point(size_t i, std::complex<float>)> phconv)
			: rdataref(pr, len, copy), idataref(pi, len, copy), magconvert(magconv), phconvert(phconv)
		{
			class ADataSource : public Plot2dPane::DataSource
			{
			public:
				const ComplexDataSourceBundle& owner;
				ADataSource(const ComplexDataSourceBundle& bun, int icolor) : owner(bun) { colorIndex = icolor; }
				virtual size_t size() const override { return owner.rdataref.size(); }
				virtual Plot2dPane::Point at(size_t i) const override
				{
					if(owner.magconvert) return owner.magconvert(i, { owner.rdataref[i], owner.idataref[i] });
					else return { (float)i, std::abs(std::complex(owner.rdataref[i], owner.idataref[i])) };
				}
			};
			class PDataSource : public Plot2dPane::DataSource
			{
			public:
				const ComplexDataSourceBundle& owner;
				PDataSource(const ComplexDataSourceBundle& bun, int icolor) : owner(bun) { colorIndex = icolor; }
				virtual size_t size() const override { return owner.rdataref.size(); }
				virtual Plot2dPane::Point at(size_t i) const override
				{
					if(owner.phconvert) return owner.phconvert(i, { owner.rdataref[i], owner.idataref[i] });
					else return { (float)i, std::arg(std::complex(owner.rdataref[i], owner.idataref[i])) };
				}
			};
			dataSourceA = new ADataSource(*this, icolor);
			dataSourceP = new PDataSource(*this, icolor);
		}
	};
	return new ComplexDataSourceBundle(icolor, pr, pi, len, copy, magconv, phconv);
}
