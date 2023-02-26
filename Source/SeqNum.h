//
//  SeqNum.h
//  generic implementation
//
//  created by yu2924 on 2023-02-24
//

#pragma once

#include <cmath>

template<typename T> struct SN10T
{
	static SN10T Ceil(T v)
	{
		SN10T sn((int)std::log10(v));
		while(sn < v) sn = sn.Next();
		return sn;
	}
	static SN10T Floor(T v)
	{
		SN10T sn = Ceil(v);
		while(v < sn) sn = sn.Prev();
		return sn;
	}
	int x; // exponent, value=10^x
	SN10T(int rx = 0) { x = rx; }
	SN10T(const SN10T& r) { x = r.x; }
	SN10T& operator=(const SN10T& r) { x = r.x; return *this; }
	SN10T Next() const { return SN10T(x + 1); }
	SN10T Prev() const { return SN10T(x - 1); }
	operator T() const { return std::pow((T)10, (T)x); }
};

using SN10F = SN10T<float>;
using SN10D = SN10T<double>;

template<typename T> struct SN125T
{
	static SN125T Ceil(T v)
	{
		SN125T sn(1, (int)std::log10(v));
		while(sn < v) sn = sn.Next();
		return sn;
	}
	static SN125T Floor(T v)
	{
		SN125T sn = Ceil(v);
		while(v < sn) sn = sn.Prev();
		return sn;
	}
	int m, x; // mantissa,exponent; value=m*10^x
	SN125T(int rm = 1, int rx = 0) { m = rm; x = rx; }
	SN125T(const SN125T& r) { m = r.m; x = r.x; }
	SN125T& operator=(const SN125T& r) { m = r.m; x = r.x; return *this; }
	int Mantissa() const { return m; }
	SN125T Next() const
	{
		switch(m)
		{
			case 1: return SN125T(2, x);
			case 2: return SN125T(5, x);
			case 5: return SN125T(1, x + 1);
		}
		return SN125T::Ceil((T)*this); // unexpected
	}
	SN125T Prev() const
	{
		switch()
		{
			case 1: return SN125T(5, x - 1);
			case 2: return SN125T(1, x);
			case 5: return SN125T(2, x);
		}
		return SN125T::Floor((T)*this); // unexpected
	}
	operator T() const
	{
		return (T)m * std::pow((T)10, (T)x);
	}
};

using SN125F = SN125T<float>;
using SN125D = SN125T<double>;

template<typename T> struct SNDecadeT
{
	static SNDecadeT Ceil(T v)
	{
		SNDecadeT sn(1, (int)std::log10(v));
		while(sn < v) sn = sn.Next();
		return sn;
	}
	static SNDecadeT Floor(T v)
	{
		SNDecadeT sn = Ceil(v);
		while(v < sn) sn = sn.Prev();
		return sn;
	}
	int m, x; // mantissa,exponent; value=m*10^x
	SNDecadeT(int rm = 1, int rx = 0) { m = rm; x = rx; }
	SNDecadeT(const SNDecadeT& r) { m = r.m; x = r.x; }
	SNDecadeT& operator=(const SNDecadeT& r) { m = r.m; x = r.x; return *this; }
	int Mantissa() const { return m; }
	SNDecadeT Next() const
	{
		return (m < 9) ? SNDecadeT(m + 1, x) : SNDecadeT(1, x + 1);
	}
	SNDecadeT Prev() const
	{
		return (1 < m) ? SNDecadeT(m - 1, x) : SNDecadeT(9, x - 1);
	}
	operator T() const
	{
		return (T)m * std::pow((T)10, (T)x);
	}
};

using SNDecadeF = SNDecadeT<float>;
using SNDecadeD = SNDecadeT<double>;
