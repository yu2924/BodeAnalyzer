//
//  SimpleFFT.h
//  generic implementation
//
//  created by yu2924 on 2023-01-23
//

#pragma once

#include <vector>
#include <cmath>
#include <complex>

template<typename T> class SimpleFFTT
{
public:
	size_t mLength, mStage;
	std::vector<T> mCosT, mSinT;
	SimpleFFTT(size_t l = 0) : mLength(0), mStage(0)
	{
		SetSize(l);
	}
	size_t GetSize() const
	{
		return mLength;
	}
	size_t SetSize(size_t l)
	{
		static const T TWOPI = (T)6.283185307179586476925286766559;
		if(mLength == l) return mLength;
		mCosT.clear();
		mSinT.clear();
		mLength = mStage = 0;
		if(l)
		{
			mLength = 1; mStage = 0; while(mLength < l) { mLength <<= 1; ++mStage; }
			mCosT.resize(mLength / 2 + 1);
			mSinT.resize(mLength / 2 + 1);
			T w = 0, dw = TWOPI / (T)mLength;
			for(size_t i = 0; i <= mLength / 2; ++i, w += dw) { mSinT[i] = sin(w); mCosT[i] = cos(w); }
		}
		return mLength;
	}
	void Transform(bool forward, std::complex<T>* pc) const
	{
		// length must be equal to mLength
		std::complex<T> c;
		// T re, im;
		size_t i, j, k, p, l, h, g, q;
		l = mLength; h = 1;
		if(forward)
		{
			for(g = 0; g < mStage; ++g)
			{
				l = l / 2; k = 0;
				for(q = 0; q < h; ++q)
				{
					p = 0;
					for(i = k; i < k + l; ++i)
					{
						j = i + l;
						c = pc[i] - pc[j];
						// re = pr[i] - pr[j];
						// im = pi[i] - pi[j];
						pc[i] += pc[j];
						// pr[i] += pr[j];
						// pi[i] += pi[j];
						pc[j] = std::complex<T>(mCosT[p], -mSinT[p]) * c;
						// pr[j] = re * mCosT[p] + im * mSinT[p];
						// pi[j] = im * mCosT[p] - re * mSinT[p];
						p += h;
					}
					k += l + l;
				}
				h += h;
			}
		}
		else
		{
			for(g = 0; g < mStage; ++g)
			{
				l = l / 2; k = 0;
				for(q = 0; q < h; ++q)
				{
					p = 0;
					for(i = k; i < k + l; ++i)
					{
						j = i + l;
						c = pc[i] - pc[j];
						// re = pr[i] - pr[j];
						// im = pi[i] - pi[j];
						pc[i] += pc[j];
						// pr[i] += pr[j];
						// pi[i] += pi[j];
						pc[j] = std::complex<T>(mCosT[p], mSinT[p]) * c;
						// pr[j] = re * mCosT[p] - im * mSinT[p];
						// pi[j] = im * mCosT[p] + re * mSinT[p];
						p += h;
					}
					k += l + l;
				}
				h += h;
			}
		}
		// bit reversal
		j = mLength / 2;
		for(i = 1; i < mLength - 1; ++i)
		{
			k = mLength;
			if(j < i)
			{
				c = pc[i];
				pc[i] = pc[j];
				pc[j] = c;
				// re = pr[i]; im = pi[i];
				// pr[i] = pr[j]; pi[i] = pi[j];
				// pr[j] = re; pi[j] = im;
			}
			while(true) { k /= 2; if(j < k) break; else j -= k; }
			j += k;
		}
		// normalize
		if(!forward)
		{
			T scale = 1 / (T)mLength;
			for(i = 0; i < mLength; ++i)
			{
				pc[i] *= scale;
				// pr[i] *= scale;
				// pi[i] *= scale;
			}
		}
	}
	void Transform(bool forward, T* pr, T* pi) const
	{
		// length must be equal to mLength
		// std::complex<T> c;
		T re, im;
		size_t i, j, k, p, l, h, g, q;
		l = mLength; h = 1;
		if(forward)
		{
			for(g = 0; g < mStage; ++g)
			{
				l = l / 2; k = 0;
				for(q = 0; q < h; ++q)
				{
					p = 0;
					for(i = k; i < k + l; ++i)
					{
						j = i + l;
						// c = pc[i] - pc[j];
						re = pr[i] - pr[j];
						im = pi[i] - pi[j];
						// pc[i] += pc[j];
						pr[i] += pr[j];
						pi[i] += pi[j];
						// pc[j] = std::complex<T>(mCosT[p], -mSinT[p]) * c;
						pr[j] = re * mCosT[p] + im * mSinT[p];
						pi[j] = im * mCosT[p] - re * mSinT[p];
						p += h;
					}
					k += l + l;
				}
				h += h;
			}
		}
		else
		{
			for(g = 0; g < mStage; ++g)
			{
				l = l / 2; k = 0;
				for(q = 0; q < h; ++q)
				{
					p = 0;
					for(i = k; i < k + l; ++i)
					{
						j = i + l;
						// c = pc[i] - pc[j];
						re = pr[i] - pr[j];
						im = pi[i] - pi[j];
						// pc[i] += pc[j];
						pr[i] += pr[j];
						pi[i] += pi[j];
						// pc[j] = std::complex<T>(mCosT[p], mSinT[p]) * c;
						pr[j] = re * mCosT[p] - im * mSinT[p];
						pi[j] = im * mCosT[p] + re * mSinT[p];
						p += h;
					}
					k += l + l;
				}
				h += h;
			}
		}
		// bit reversal
		j = mLength / 2;
		for(i = 1; i < mLength - 1; ++i)
		{
			k = mLength;
			if(j < i)
			{
				// c = pc[i];
				// pc[i] = pc[j];
				// pc[j] = c;
				re = pr[i]; im = pi[i];
				pr[i] = pr[j]; pi[i] = pi[j];
				pr[j] = re; pi[j] = im;
			}
			while(true) { k /= 2; if(j < k) break; else j -= k; }
			j += k;
		}
		// normalize
		if(!forward)
		{
			T scale = 1 / (T)mLength;
			for(i = 0; i < mLength; ++i)
			{
				// pc[i] *= scale;
				pr[i] *= scale;
				pi[i] *= scale;
			}
		}
	}
};

using SimpleFFTF = SimpleFFTT<float>;
using SimpleFFTD = SimpleFFTT<double>;
