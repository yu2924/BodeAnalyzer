//
//  SimpleConv.h
//  generic implementation
//
//  created by yu2924 on 2023-02-25
//

#pragma once

#include <vector>
#include "SimpleFFT.h"

template<typename T> class SimpleConvT
{
public:
	static void vclear(T* pd, size_t l)
	{
		for(size_t i = 0; i < l; ++i) pd[i] = 0;
	}
	static void vcopy(const T* ps, T* pd, size_t l)
	{
		for(size_t i = 0; i < l; ++i) pd[i] = ps[i];
	}
	static void vadd(const T* ps, T* pd, size_t l)
	{
		for(size_t i = 0; i < l; ++i) pd[i] += ps[i];
	}
	static void vcmul(const T* psr, const T* psi, T* pdr, T* pdi, size_t l)
	{
		T dr, di; for(size_t i = 0; i < l; ++i)
		{
			dr = pdr[i]; di = pdi[i];
			pdr[i] = psr[i] * dr - psi[i] * di;
			pdi[i] = psi[i] * dr + psr[i] * di;
		}
	}
	std::vector<T> mHre, mHim, mVreFore, mVreBack, mVim;
	SimpleFFTT<T> mFFT;
	size_t mLength = 0;
	size_t mBufPos = 0;
	SimpleConvT(size_t l = 0)
	{
		SetLength(l);
	}
	size_t GetLength() const
	{
		return mLength;
	}
	void SetLength(size_t l)
	{
		mLength = 0;
		if(0 < l) { mLength = 4; while(mLength < l) mLength <<= 1; }
		mHre.assign(mLength * 2, 0);
		mHim.assign(mLength * 2, 0);
		mVreFore.assign(mLength * 2, 0);
		mVreBack.assign(mLength * 2, 0);
		mVim.assign(mLength * 2, 0);
		mFFT.SetSize(mLength * 2);
		mBufPos = 0;
	}
	void SetIR(const T* p, size_t l)
	{
		l = std::min(mLength, l);
		vcopy(p, mHre.data(), l);
		vclear(mHre.data() + l, mLength * 2 - l);
		vclear(mHim.data(), mLength * 2);
		mFFT.Transform(true, mHre.data(), mHim.data());
	}
	bool SetFR(const T* pr, const T* pi, size_t l)
	{
		if(mLength * 2 != l) return false;
		vcopy(pr, mHre.data(), l);
		vcopy(pi, mHim.data(), l);
		return true;
	}
	void Reset(size_t bufpos = 0)
	{
		vclear(mVreFore.data(), mVreFore.size());
		vclear(mVreBack.data(), mVreBack.size());
		vclear(mVim.data(), mVim.size());
		mBufPos = bufpos & (mLength - 1);
	}
	void InternalTransform()
	{
		// prepare input
		vclear(mVreFore.data() + mLength, mLength);
		vclear(mVim.data(), mLength * 2);
		// fft => mul => ifft
		mFFT.Transform(true, mVreFore.data(), mVim.data());
		vcmul(mHre.data(), mHim.data(), mVreFore.data(), mVim.data(), mLength * 2);
		mFFT.Transform(false, mVreFore.data(), mVim.data());
		// overlap-add
		vadd(mVreBack.data() + mLength, mVreFore.data(), mLength);
		mVreFore.swap(mVreBack);
	}
	T Process(T vi)
	{
		mVreFore[mBufPos] = vi;
		T vo = mVreBack[mBufPos];
		++mBufPos;
		if(mLength <= mBufPos)
		{
			mBufPos = 0;
			InternalTransform();
		}
		return vo;
	}
	void Process(const T* pi, T* po, size_t len)
	{
		size_t pos = 0;
		while(pos < len)
		{
			size_t lseg = std::min(len - pos, mLength - mBufPos);
			vcopy(pi + pos, mVreFore.data() + mBufPos, lseg);
			vcopy(mVreBack.data() + mBufPos, po + pos, lseg);
			pos += lseg;
			mBufPos += lseg;
			if(mLength <= mBufPos)
			{
				mBufPos = 0;
				InternalTransform();
			}
		}
	}
	void Process(T* p, size_t l)
	{
		Process(p, p, l);
	}
};

using SimpleConvF = SimpleConvT<float>;
using SimpleConvD = SimpleConvT<double>;
