//
//  MagPhasePlotPane.h
//  simple 2D plot library
//
//  created by yu2924 on 2023-01-26
//

#pragma once

#include "Plot2dPane.h"
#include <complex>

class MagPhasePlotPane : public juce::Component
{
public:
	class DataSourceBundle : public juce::ReferenceCountedObject
	{
	public:
		using Ptr = juce::ReferenceCountedObjectPtr<DataSourceBundle>;
		Plot2dPane::DataSource::Ptr dataSourceA;
		Plot2dPane::DataSource::Ptr dataSourceP;
	};
protected:
	DataSourceBundle::Ptr dataSourceBundle;
	Plot2dPane plotPaneA, plotPaneP;
public:
	MagPhasePlotPane();
	virtual void resized() override;
	virtual void paint(juce::Graphics&) override {}
	void setAxes(const Plot2dPane::Axis& axisx, const Plot2dPane::Axis& axisa, const Plot2dPane::Axis& axisp);
	void setAxes(const juce::String& xlabel, Plot2dPane::Range xrange, const std::vector<float>& xticks, bool xlog, std::function<juce::String(float)> xTextFromValue,
				 const juce::String& alabel, Plot2dPane::Range arange, const std::vector<float>& aticks, bool alog, std::function<juce::String(float)> aTextFromValue,
				 const juce::String& plabel, Plot2dPane::Range prange, const std::vector<float>& pticks, bool plog, std::function<juce::String(float)> pTextFromValue);
	void clearDataSourceBundle();
	void setDataSourceBundle(DataSourceBundle::Ptr dsb);
	// generic data source bundle builder
	static DataSourceBundle::Ptr createDataSourceBundle(int icolor, const std::complex<double>* pc, size_t len, bool copy,
		std::function<Plot2dPane::Point(size_t i, std::complex<double>)> magconv,
		std::function<Plot2dPane::Point(size_t i, std::complex<double>)> phconv);
	static DataSourceBundle::Ptr createDataSourceBundle(int icolor, const std::complex<float>* pc, size_t len, bool copy,
		std::function<Plot2dPane::Point(size_t i, std::complex<float>)> magconv,
		std::function<Plot2dPane::Point(size_t i, std::complex<float>)> phconv);
	static DataSourceBundle::Ptr createDataSourceBundle(int icolor, const float* pr, const float* pi, size_t len, bool copy,
		std::function<Plot2dPane::Point(size_t i, std::complex<float>)> magconv,
		std::function<Plot2dPane::Point(size_t i, std::complex<float>)> phconv);
};
