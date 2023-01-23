//
//  Plot2dPanes.h
//  simple 2D plot library
//
//  created by yu2924 on 2023-01-23
//

#pragma once
#include <JuceHeader.h>
#include <vector>
#include <complex>

class Plot2dPane : public juce::Component
{
protected:
	class Impl;
	Impl* impl;
public:
	static constexpr int ColorIndexAuto = -1;
	enum TickFlags
	{
		None	= 0x00,
		Minor	= 0x01,
		Major	= 0x10,
		Both	= 0x11,
	};
	struct Range
	{
		float lval, hval;
		bool isEmpty() const { return lval == hval; }
		Range normalized() const { return { std::min(lval, hval), std::max(lval, hval) }; }
		bool intersectsIncludeEdge(float v) const { auto r = normalized(); return (r.lval <= v) && (v <= r.hval); }
		bool intersectsExcludeEdge(float v) const { auto r = normalized(); return (r.lval < v) && (v < r.hval); }
		bool intersectsEdge(float v) const { return (lval == v) || (hval == v); }
		Range operator|(Range r) const { return { std::min(lval, r.lval), std::max(hval, r.hval) }; }
	};
	struct RangeXY
	{
		Range rangex, rangey;
	};
	struct Point
	{
		float x, y;
		operator juce::Point<float>() const { return { x,y }; }
	};
	struct Axis
	{
		juce::String label;
		Range range{ 0, 1 };
		std::vector<float> majTicks;
		std::vector<float> minTicks;
		TickFlags tickFlags = Both;
		TickFlags gridFlags = Major;
		TickFlags tickLabelFlags = Major;
		bool logscale = false;
		bool labelVisible = true;
		std::function<juce::String(float)> textFromValue; // vdisp->text
		std::vector<float> majTickArray() const { return majTicks.empty() ? std::vector<float>{ range.lval, range.hval } : majTicks; }
		juce::String formatValue(float v) const { return textFromValue ? textFromValue(v) : juce::String(v); }
	};
	class DataSource : public juce::ReferenceCountedObject
	{
	public:
		using Ptr = juce::ReferenceCountedObjectPtr<DataSource>;
		using PtrArray = juce::ReferenceCountedArray<DataSource>;
		int colorIndex = ColorIndexAuto;
		virtual size_t size() const = 0;
		virtual Point at(size_t i) const = 0;
		Point operator[](size_t i) const { return at(i); }
		virtual RangeXY findRange() const
		{
			float xmin, xmax, ymin, ymax;
			Point p = at(0);
			xmin = xmax = p.x;
			ymin = ymax = p.y;
			for(size_t c = size(), i = 1; i < c; ++i)
			{
				p = at(i);
				xmin = std::min(xmin, p.x); xmax = std::max(xmax, p.x);
				ymin = std::min(ymin, p.y); ymax = std::max(ymax, p.y);
			}
			return { { xmin, xmax }, { ymin, ymax } };
		}
	};
	template<typename T> struct DataRef
	{
		const T* ptr;
		size_t len;
		std::vector<T> ownbuf;
		DataRef(const T* p, size_t l, bool copy) : ptr(nullptr), len(l)
		{
			if(copy) { ownbuf.assign(p, p + l); ptr = ownbuf.data(); }
			else { ptr = p; }
		}
		size_t size() const { return len; }
		const T* data() const { return ptr; }
		const T* operator->() const { return ptr; }
		const T& operator[](size_t i) const { return ptr[i]; }
	};
	Plot2dPane();
	virtual ~Plot2dPane();
	virtual void resized() override;
	virtual void paint(juce::Graphics& g) override;
	const Axis& getAxisX() const;
	const Axis& getAxisY() const;
	void setAxisX(const Axis& v);
	void setAxisY(const Axis& v);
	juce::BorderSize<int> getLabelBorder() const;
	void setLabelBorder(const juce::BorderSize<int>& v);
	void adjustLabelBorder();
	DataSource::PtrArray getDataSources() const;
	void clearDataSources();
	void removeDataSource(DataSource::Ptr v);
	void addDataSource(DataSource::Ptr v);
	void setDataSource(DataSource::Ptr v);
	void adjustRangesToFit(bool adjustx, bool adjusty);
	// quick methods
	void setAxes(const juce::String& xlabel, Range xrange, const std::vector<float>& xticks, bool xlog, std::function<juce::String(float)> xTextFromValue,
				 const juce::String& ylabel, Range yrange, const std::vector<float>& yticks, bool ylog, std::function<juce::String(float)> yTextFromValue);
	// generic data source builder
	static DataSource::Ptr createFloatArrayDataSource(int icolor, const float* pv, size_t len, bool copy, std::function<Point(size_t, float)> conv);
	static DataSource::Ptr createComplexArrayDataSource(int icolor, const std::complex<double>* pc, size_t len, bool copy, std::function<Point(size_t, std::complex<double>)> conv);
	static DataSource::Ptr createComplexArrayDataSource(int icolor, const std::complex<float>* pc, size_t len, bool copy, std::function<Point(size_t, std::complex<float>)> conv);
	static DataSource::Ptr createComplexArrayDataSource(int icolor, const float* pr, const float* pi, size_t len, bool copy, std::function<Point(size_t, std::complex<float>)> conv);
	// generic axis builder
	static Axis createAxisForLogDec(const juce::String& label, Range range, bool setmin);
	static Axis createAxisForLin(const juce::String& label, Range range, bool setmin);
};
