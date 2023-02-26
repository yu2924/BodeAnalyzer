//
//  Plot2dPanes.cpp
//  simple 2D plot library
//
//  created by yu2924 on 2023-01-23
//

#include "Plot2dPane.h"
#include <cmath>
#include <algorithm>

// ================================================================================
// helpers

class MapBase : public juce::ReferenceCountedObject
{
public:
	using Ptr = juce::ReferenceCountedObjectPtr<MapBase>;
	virtual float Map(float v) const = 0;
	virtual float Unmap(float v) const = 0;
};

class MapLin : public MapBase
{
protected:
	float ao, ar, bo, br;
	float invar, invbr;
public:
	MapLin(float al = 0, float ah = 1, float bl = 0, float bh = 1) { ao = al; ar = ah - al; bo = bl; br = bh - bl; invar = 1 / ar; invbr = 1 / br; }
	virtual float Map(float v) const override { return bo + (v - ao) * br * invar; }
	virtual float Unmap(float v) const override { return ao + (v - bo) * ar * invbr; }
};

class MapExp : public MapBase
{
protected:
	float ao, ar, pw, sc;
	float invar, invpw, invsc;
public:
	MapExp(float al = 0, float ah = 1, float bl = 0.001f, float bh = 1) { ao = al; ar = ah - al; pw = std::log(bh) - std::log(bl); sc = bl; invar = 1 / ar; invpw = 1 / pw; invsc = 1 / sc; }
	virtual float Map(float v) const override { return std::exp((v - ao) * invar * pw) * sc; }
	virtual float Unmap(float v) const override { return ao + ar * std::log(v * invsc) * invpw; }
};

// ================================================================================
// Plot2dPane

class Plot2dPane::Impl
{
public:
	struct ColorTable
	{
		const std::vector<juce::Colour> table =
		{
			// tab10
			juce::Colour{0xff1f77b4}, // blue
			juce::Colour{0xffff7f0e}, // orange
			juce::Colour{0xff2ca02c}, // green
			juce::Colour{0xffd62728}, // red
			juce::Colour{0xff9467bd}, // purple
			juce::Colour{0xff8c564b}, // brown
			juce::Colour{0xffe377c2}, // pink
			juce::Colour{0xff7f7f7f}, // gray
			juce::Colour{0xffbcbd22}, // olive
			juce::Colour{0xff17becf}, // cyan
		};
		size_t size()
		{
			return table.size();
		}
		juce::Colour resolveColor(int i)
		{
			return ((size_t)i < table.size()) ? table[i] : juce::Colours::black;
		}
	};
	Plot2dPane* mOwner;
	juce::Font mFont{ 13.0f };
	int mMargin = 4;
	int mPlotMargin = 8;
	juce::Colour mBackColor{ 0xffffffff };
	juce::Colour mLabelColor{ 0xff000000 };
	juce::Colour mTickColor{ 0xff000000 };
	juce::Colour mGridColor{ 0xff808080 };
	MapBase::Ptr mMapP2LX, mMapP2LY;
	Axis mAxisX;
	Axis mAxisY;
	juce::BorderSize<int> mLabelBorder;
	DataSource::PtrArray mDataSources;
	juce::SharedResourcePointer<ColorTable> mColorTable;
	Impl(Plot2dPane* p) : mOwner(p)
	{
		mOwner->setOpaque(true);
	}
	void updateMapXY()
	{
		juce::Rectangle<int> rc = mOwner->getLocalBounds().reduced(mMargin);
		juce::Rectangle<int> rcp = mLabelBorder.subtractedFrom(rc).reduced(mPlotMargin);
		if(mAxisX.range.isEmpty()) { mAxisX.range.lval -= std::numeric_limits<float>::epsilon(); mAxisX.range.hval += std::numeric_limits<float>::epsilon(); }
		if(mAxisY.range.isEmpty()) { mAxisY.range.lval -= std::numeric_limits<float>::epsilon(); mAxisY.range.hval += std::numeric_limits<float>::epsilon(); }
		mMapP2LX.reset();
		mMapP2LY.reset();
		if(!rcp.isEmpty())
		{
			if(mAxisX.logscale) mMapP2LX = new MapExp((float)rcp.getX(), (float)rcp.getRight(), mAxisX.range.lval, mAxisX.range.hval);
			else				mMapP2LX = new MapLin((float)rcp.getX(), (float)rcp.getRight(), mAxisX.range.lval, mAxisX.range.hval);
			if(mAxisY.logscale) mMapP2LY = new MapExp((float)rcp.getBottom(), (float)rcp.getY(), mAxisY.range.lval, mAxisY.range.hval);
			else				mMapP2LY = new MapLin((float)rcp.getBottom(), (float)rcp.getY(), mAxisY.range.lval, mAxisY.range.hval);
		}
	}
	void repaintPlotRect()
	{
		juce::Rectangle<int> rc = mOwner->getLocalBounds().reduced(mMargin);
		juce::Rectangle<int> rcf = mLabelBorder.subtractedFrom(rc);
		juce::Rectangle<int> rcp = rcf.reduced(mPlotMargin);
		mOwner->repaint(rcp.reduced(-1));
	}
	int chooseNextColorIndex()
	{
		std::vector<int> usecount(mColorTable->size());
		for(const auto& ds : mDataSources)
		{
			int ic = ds->colorIndex;
			if((size_t)ic < usecount.size()) usecount[ic] ++;
		}
		std::vector<int>::iterator it = std::min_element(usecount.begin(), usecount.end());
		return (int)std::distance<std::vector<int>::iterator>(usecount.begin(), it);
	}
	void resized()
	{
		updateMapXY();
	}
	void paint(juce::Graphics& g)
	{
		juce::Rectangle<int> rc = mOwner->getLocalBounds().reduced(mMargin);
		juce::Rectangle<int> rcf = mLabelBorder.subtractedFrom(rc);
		juce::Rectangle<int> rcp = rcf.reduced(mPlotMargin);
		// background
		g.setColour(mBackColor);
		g.fillAll();
		// labels & grids
		g.setColour(mGridColor);
		g.drawRect(rcf, 1);
		float fontheight = mFont.getHeight();
		float fontascent = mFont.getAscent();
		int textheight = juce::roundToInt(fontheight + 0.5f);
		static const float dash[] = { 2, 2 };
		juce::Colour mingridcolor = mGridColor.interpolatedWith(mBackColor, 0.5f);
		// x labels & grids
		{
			Range rng = mAxisX.range.normalized();
			int ytbaseline = rcf.getBottom() + textheight / 2 + juce::roundToInt(fontascent);
			for(const auto& v : mAxisX.majTickArray())
			{
				if(!rng.intersectsIncludeEdge(v)) continue;
				float xtick = mMapP2LX->Unmap(v);
				// tick
				if(mAxisX.tickFlags & Major)
				{
					g.setColour(mTickColor);
					g.drawLine({ xtick, (float)rcf.getBottom(), xtick, (float)rcf.getBottom() + fontheight * 0.4f }, 1);
				}
				// grid
				if((mAxisX.gridFlags & Major) && !rng.intersectsEdge(v))
				{
					g.setColour(mGridColor);
					g.drawLine({ xtick, (float)rcf.getY(), xtick, (float)rcf.getBottom() }, 1);
				}
				// tick label
				if(mAxisX.tickLabelFlags & Major)
				{
					g.setColour(mLabelColor);
					g.drawSingleLineText(mAxisX.formatValue(v), juce::roundToInt(xtick), ytbaseline, juce::Justification::horizontallyCentred);
				}
			}
			for(const auto& v : mAxisX.minTicks)
			{
				if(!rng.intersectsIncludeEdge(v)) continue;
				float xtick = mMapP2LX->Unmap(v);
				// tick
				if(mAxisX.tickFlags & Minor)
				{
					g.setColour(mTickColor);
					g.drawLine({ xtick, (float)rcf.getBottom(), xtick, (float)rcf.getBottom() + fontheight * 0.2f }, 1);
				}
				// grid
				if((mAxisX.gridFlags & Minor) && (v != rng.lval) && (v != rng.hval))
				{
					g.setColour(mingridcolor);
					g.drawDashedLine({ xtick, (float)rcf.getY(), xtick, (float)rcf.getBottom() }, dash, 2, 1, 0);
				}
				// tick label
				if(mAxisX.tickLabelFlags & Minor)
				{
					g.setColour(mLabelColor);
					g.drawSingleLineText(mAxisX.formatValue(v), juce::roundToInt(xtick), ytbaseline, juce::Justification::horizontallyCentred);
				}
			}
			if(mAxisX.labelVisible)
			{
				int ybaseline = rc.getBottom() + juce::roundToInt(fontascent - fontheight);
				g.setColour(mLabelColor);
				g.drawSingleLineText(mAxisX.label, rcf.getCentreX(), ybaseline, juce::Justification::horizontallyCentred);
			}
		}
		// y labels & grids
		{
			Range rng = mAxisY.range.normalized();
			int xright = rcf.getX() - textheight / 2;
			for(const auto& v : mAxisY.majTickArray())
			{
				if(!rng.intersectsIncludeEdge(v)) continue;
				float ytick = mMapP2LY->Unmap(v);
				// tick
				if(mAxisY.tickFlags & Major)
				{
					g.setColour(mTickColor);
					g.drawLine({ (float)rcf.getX(), ytick, (float)rcf.getX() - fontheight * 0.4f, ytick }, 1);
				}
				// grid
				if((mAxisY.gridFlags & Major) && !rng.intersectsEdge(v))
				{
					g.setColour(mGridColor);
					g.drawLine({ (float)rcf.getX(), ytick, (float)rcf.getRight(), ytick }, 1);
				}
				// tick labl
				if(mAxisY.tickLabelFlags & Major)
				{
					g.setColour(mLabelColor);
					g.drawSingleLineText(mAxisY.formatValue(v), xright, juce::roundToInt(ytick - fontheight * 0.5f + fontascent), juce::Justification::right);
				}
			}
			for(const auto& v : mAxisY.minTicks)
			{
				if(!rng.intersectsIncludeEdge(v)) continue;
				float ytick = mMapP2LY->Unmap(v);
				// tick
				if(mAxisY.tickFlags & Minor)
				{
					g.setColour(mTickColor);
					g.drawLine({ (float)rcf.getX(), ytick, (float)rcf.getX() - fontheight * 0.2f, ytick }, 1);
				}
				// grid
				if((mAxisY.gridFlags & Minor) && (v != rng.lval) && (v != rng.hval))
				{
					g.setColour(mingridcolor);
					g.drawDashedLine({ (float)rcf.getX(), ytick, (float)rcf.getRight(), ytick }, dash, 2, 1, 0);
				}
				// tick labl
				if(mAxisY.tickLabelFlags & Minor)
				{
					g.setColour(mLabelColor);
					g.drawSingleLineText(mAxisY.formatValue(v), xright, juce::roundToInt(ytick - fontheight * 0.5f + fontascent), juce::Justification::right);
				}
			}
			if(mAxisY.labelVisible)
			{
				g.saveState();
				juce::AffineTransform xfm = juce::AffineTransform::rotation(-juce::float_Pi * 0.5f);
				juce::Point<float> pt{ (float)rc.getX() + fontascent, (float)rcf.getCentreY() };
				xfm.inverted().transformPoint(pt.x, pt.y);
				g.addTransform(xfm);
				g.setColour(mLabelColor);
				g.drawSingleLineText(mAxisY.label, juce::roundToInt(pt.x), juce::roundToInt(pt.y), juce::Justification::horizontallyCentred);
				g.restoreState();
			}
		}
		// data
		if(!mDataSources.isEmpty() && g.clipRegionIntersects(rcp.reduced(-1)))
		{
			g.saveState();
			g.reduceClipRegion(rcp);
			for(const auto& ds : mDataSources)
			{
				g.setColour(mColorTable->resolveColor(ds->colorIndex));
				juce::Path path;
				path.preallocateSpace(64);
				int numpt = 0;
				size_t c = ds->size(), i = 0;
				for(; i < c; ++i)
				{
					Point pt = ds->at(i);
					pt.x = mMapP2LX->Unmap(pt.x);
					pt.y = mMapP2LY->Unmap(pt.y);
					if(std::isnormal(pt.x) && std::isnormal(pt.y))
					{
						path.startNewSubPath(pt.x, pt.y);
						numpt = 1;
						break;
					}
				}
				for(; i < c; ++i)
				{
					Point pt = ds->at(i);
					pt.x = mMapP2LX->Unmap(pt.x);
					pt.y = mMapP2LY->Unmap(pt.y);
					if(std::isnormal(pt.x) && std::isnormal(pt.y))
					{
						path.lineTo(pt.x, pt.y);
						++numpt;
						if(64 <= numpt) // split drawing to reduce memory spikes
						{
							g.strokePath(path, juce::PathStrokeType(1, juce::PathStrokeType::curved));
							path.clear();
							path.preallocateSpace(64);
							path.startNewSubPath(pt.x, pt.y);
							numpt = 1;
						}
					}
				}
				if(2 <= numpt)
				{
					g.strokePath(path, juce::PathStrokeType(1, juce::PathStrokeType::curved));
				}
			}
			g.restoreState();
		}
	}
	const Axis& getAxisX() const { return mAxisX; }
	const Axis& getAxisY() const { return mAxisY; }
	void setAxisX(const Axis& v) { mAxisX = v; updateMapXY(); mOwner->repaint(); }
	void setAxisY(const Axis& v) { mAxisY = v; updateMapXY(); mOwner->repaint(); }
	juce::BorderSize<int> getLabelBorder() const { return mLabelBorder; }
	void setLabelBorder(const juce::BorderSize<int>& v) { mLabelBorder = v; updateMapXY(); mOwner->repaint(); }
	void adjustLabelBorder()
	{
		std::function<int(float)> textwidthx = [this](float v)
		{
			juce::String s = mAxisX.formatValue(v);
			return juce::roundToInt(mFont.getStringWidthFloat(s) + 0.5f);
		};
		std::function<int(float)> textwidthy = [this](float v)
		{
			juce::String s = mAxisY.formatValue(v);
			return juce::roundToInt(mFont.getStringWidthFloat(s) + 0.5f);
		};
		int textheight = juce::roundToInt(mFont.getHeight() + 0.5f);
		int lwx = 0; for(const auto& v : { mAxisX.range.lval, mAxisX.range.hval }) lwx = std::max(lwx, textwidthx(v));
		int lwy = 0; for(const auto& v : { mAxisY.range.lval, mAxisY.range.hval }) lwy = std::max(lwy, textwidthy(v));
		juce::BorderSize<int> border;
		border.setLeft(textheight / 2 + ((mAxisY.tickLabelFlags & Both) ? lwy : 0) + (mAxisY.labelVisible ? textheight : 0));
		border.setBottom(textheight / 2 + ((mAxisX.tickLabelFlags & Both) ? textheight : 0) + (mAxisX.labelVisible ? textheight : 0));
		border.setTop(textheight / 2);
		border.setRight(lwx / 2);
		setLabelBorder(border);
	}
	DataSource::PtrArray getDataSources() const { return mDataSources; }
	void clearDataSources()
	{
		mDataSources.clear();
		repaintPlotRect();
	}
	void removeDataSource(DataSource::Ptr v)
	{
		mDataSources.removeObject(v);
		repaintPlotRect();
	}
	void addDataSource(DataSource::Ptr v)
	{
		if(!v) return;
		if(mDataSources.contains(v)) return;
		if(v->colorIndex == ColorIndexAuto) v->colorIndex = chooseNextColorIndex();
		mDataSources.add(v);
		repaintPlotRect();
	}
	void setDataSource(DataSource::Ptr v)
	{
		mDataSources.clear();
		if(v) addDataSource(v);
	}
	void adjustRangesToFit(bool adjustx, bool adjusty)
	{
		if(mDataSources.isEmpty()) return;
		RangeXY rngxy = mDataSources[0]->findRange();
		for(int c = mDataSources.size(), i = 1; i < c; ++i)
		{
			RangeXY rxy = mDataSources[i]->findRange();
			rngxy.rangex = rngxy.rangex | rxy.rangex;
			rngxy.rangey = rngxy.rangey | rxy.rangey;
		}
		if(adjustx) mAxisX.range = rngxy.rangex;
		if(adjusty) mAxisY.range = rngxy.rangey;
		adjustLabelBorder();
	}
};

Plot2dPane::Plot2dPane() { impl = new Impl(this); }
Plot2dPane::~Plot2dPane() { delete impl; }
void Plot2dPane::resized() { impl->resized(); }
void Plot2dPane::paint(juce::Graphics& g) { impl->paint(g); }
const Plot2dPane::Axis& Plot2dPane::getAxisX() const { return impl->getAxisX(); }
const Plot2dPane::Axis& Plot2dPane::getAxisY() const { return impl->getAxisY(); }
void Plot2dPane::setAxisX(const Axis& v) { impl->setAxisX(v); }
void Plot2dPane::setAxisY(const Axis& v) { impl->setAxisY(v); }
juce::BorderSize<int> Plot2dPane::getLabelBorder() const { return impl->getLabelBorder(); }
void Plot2dPane::setLabelBorder(const juce::BorderSize<int>& v) { impl->setLabelBorder(v); }
void Plot2dPane::adjustLabelBorder() { impl->adjustLabelBorder(); }
Plot2dPane::DataSource::PtrArray Plot2dPane::getDataSources() const { return impl->getDataSources(); }
void Plot2dPane::clearDataSources() { impl->clearDataSources(); }
void Plot2dPane::removeDataSource(DataSource::Ptr v) { impl->removeDataSource(v); }
void Plot2dPane::addDataSource(DataSource::Ptr v) { impl->addDataSource(v); }
void Plot2dPane::setDataSource(DataSource::Ptr v) { impl->setDataSource(v); }
void Plot2dPane::adjustRangesToFit(bool adjustx, bool adjusty) { impl->adjustRangesToFit(adjustx, adjusty); }

// ================================================================================
// quick methods

void Plot2dPane::setAxes(const juce::String& xlabel, Range xrange, const std::vector<float>& xticks, bool xlog, std::function<juce::String(float)> xTextFromValue,
	const juce::String& ylabel, Range yrange, const std::vector<float>& yticks, bool ylog, std::function<juce::String(float)> yTextFromValue)
{
	Plot2dPane::Axis axisx;
	axisx.label = xlabel;
	axisx.range = xrange;
	axisx.majTicks = xticks;
	axisx.minTicks = {};
	axisx.tickFlags = Both;
	axisx.gridFlags = Major;
	axisx.tickLabelFlags = Major;
	axisx.logscale = xlog;
	axisx.labelVisible = xlabel.isNotEmpty();
	axisx.textFromValue = xTextFromValue;
	Plot2dPane::Axis axisy;
	axisy.label = ylabel;
	axisy.range = yrange;
	axisy.majTicks = yticks;
	axisy.minTicks = {};
	axisy.tickFlags = Both;
	axisy.gridFlags = Major;
	axisy.tickLabelFlags = Major;
	axisy.logscale = ylog;
	axisy.labelVisible = ylabel.isNotEmpty();
	axisy.textFromValue = yTextFromValue;
	setAxisX(axisx);
	setAxisY(axisy);
	adjustLabelBorder();
}

// ================================================================================
// generic data source builder

Plot2dPane::DataSource::Ptr Plot2dPane::createFloatArrayDataSource(int icolor, const float* pv, size_t len, bool copy, std::function<Point(size_t, float)> conv)
{
	class FloatArrayDataSource : public Plot2dPane::DataSource
	{
	protected:
		DataRef<float> dataref;
		std::function<Point(size_t, float)> convert;
	public:
		FloatArrayDataSource(int icolor, const float* pv, size_t len, bool copy, std::function<Point(size_t, float)> conv) : dataref(pv, len, copy), convert(conv) { colorIndex = icolor; }
		virtual size_t size() const override { return dataref.size(); }
		virtual Point at(size_t i) const override
		{
			if(convert) return convert(i, dataref[i]);
			else  return { (float)i, dataref[i] };
		}
	};
	return new FloatArrayDataSource(icolor, pv, len, copy, conv);
}

Plot2dPane::DataSource::Ptr Plot2dPane::createFloatArrayDataSource(int icolor, const double* pv, size_t len, bool copy, std::function<Point(size_t, double)> conv)
{
	class FloatArrayDataSource : public Plot2dPane::DataSource
	{
	protected:
		DataRef<double> dataref;
		std::function<Point(size_t, double)> convert;
	public:
		FloatArrayDataSource(int icolor, const double* pv, size_t len, bool copy, std::function<Point(size_t, double)> conv) : dataref(pv, len, copy), convert(conv) { colorIndex = icolor; }
		virtual size_t size() const override { return dataref.size(); }
		virtual Point at(size_t i) const override
		{
			if(convert) return convert(i, dataref[i]);
			else  return { (float)i, (float)dataref[i] };
		}
	};
	return new FloatArrayDataSource(icolor, pv, len, copy, conv);
}

Plot2dPane::DataSource::Ptr Plot2dPane::createComplexArrayDataSource(int icolor, const std::complex<double>* pc, size_t len, bool copy, std::function<Point(size_t, std::complex<double>)> conv)
{
	class ComplexArrayDataSource : public Plot2dPane::DataSource
	{
	protected:
		DataRef<std::complex<double> > dataref;
		std::function<Point(size_t, std::complex<double>)> convert;
	public:
		ComplexArrayDataSource(int icolor, const std::complex<double>* pc, size_t len, bool copy, std::function<Point(size_t, std::complex<double>)> conv) : dataref(pc, len, copy), convert(conv) { colorIndex = icolor; }
		virtual size_t size() const override { return dataref.size(); }
		virtual Point at(size_t i) const override
		{
			if(convert) return convert(i, dataref[i]);
			return { (float)i, (float)std::abs(dataref[i]) };
		}
	};
	return new ComplexArrayDataSource(icolor, pc, len, copy, conv);
}

Plot2dPane::DataSource::Ptr Plot2dPane::createComplexArrayDataSource(int icolor, const std::complex<float>* pc, size_t len, bool copy, std::function<Point(size_t, std::complex<float>)> conv)
{
	class ComplexArrayDataSource : public Plot2dPane::DataSource
	{
	protected:
		DataRef<std::complex<float> > dataref;
		std::function<Point(size_t, std::complex<float>)> convert;
	public:
		ComplexArrayDataSource(int icolor, const std::complex<float>* pc, size_t len, bool copy, std::function<Point(size_t, std::complex<float>)> conv) : dataref(pc, len, copy), convert(conv) { colorIndex = icolor; }
		virtual size_t size() const override { return dataref.size(); }
		virtual Point at(size_t i) const override
		{
			if(convert) return convert(i, dataref[i]);
			return { (float)i, std::abs(dataref[i]) };
		}
	};
	return new ComplexArrayDataSource(icolor, pc, len, copy, conv);
}

Plot2dPane::DataSource::Ptr Plot2dPane::createComplexArrayDataSource(int icolor, const float* pr, const float* pi, size_t len, bool copy, std::function<Point(size_t, std::complex<float>)> conv)
{
	class ComplexArrayDataSource : public Plot2dPane::DataSource
	{
	protected:
		DataRef<float> rdataref;
		DataRef<float> idataref;
		std::function<Point(size_t, std::complex<float>)> convert;
	public:
		ComplexArrayDataSource(int icolor, const float* pr, const float* pi, size_t len, bool copy, std::function<Point(size_t, std::complex<float>)> conv) : rdataref(pr, len, copy), idataref(pi, len, copy), convert(conv) { colorIndex = icolor; }
		virtual size_t size() const override { return rdataref.size(); }
		virtual Point at(size_t i) const override
		{
			if(convert) return convert(i, std::complex<float>(rdataref[i], idataref[i]));
			return { (float)i, std::abs(std::complex<float>(rdataref[i], idataref[i])) };
		}
	};
	return new ComplexArrayDataSource(icolor, pr, pi, len, copy, conv);
}
