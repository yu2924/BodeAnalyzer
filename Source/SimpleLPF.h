//
//  SimpleLPF.h
//  generic implementation
//
//  created by yu2924 on 2023-02-23
//

#pragma once

//
// Transposed Direct-Form II
// 
//             b0
// x[n] ----+--|>--|+|------+----y[n]
//          |       | s1    |
//          |     |z-1|     |
//          |  b1   |  -a1  |
//          +--|>--|+|--<|--+
//          |       | s2    |
//          |     |z-1|     |
//          |  b2   |  -a2  |
//          +--|>--|+|--<|--+
// 
//        b0 + b1*z^-1 + b2*z^-2   b0*Z^2 + b1*z + b2
// H(z) = ---------------------- = ------------------
//         1 + a1*z^-1 + a2*z^-2      Z^2 + a1*z + a2
// 
// y = b0*x + s1; s1 = b1*x - a1*y + s2; s2 = b2*x - a2*y;
//
template<typename T> class SimpleLPFT
{
public:
	struct Coef { T a1, a2, b0, b1, b2; } coef = { 0, 0, 1, 0, 0 };
	T s1 = 0, s2 = 0;
	SimpleLPFT(T f = 0, T q = (T)0.70710678118654752440084436210485)
	{
		SetFQ(f, q);
	}
	void SetFQ(T f, T q)
	{
		// based on : http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt
		constexpr T TWOPI = (T)6.283185307179586476925286766559;
		T w = TWOPI * f, c = std::cos(w), s = std::sin(w);
		T alpha = s / (2 * q);
		T a0 = 1 + alpha, inva0 = 1 / a0;
		coef.a1 = (-2 * c) * inva0;
		coef.a2 = (1 - alpha) * inva0;
		coef.b0 = ((1 - c) * 0.5f) * inva0;
		coef.b1 = (1 - c) * inva0;
		coef.b2 = ((1 - c) * 0.5f) * inva0;
	}
	void Reset()
	{
		s1 = s2 = 0;
	}
	T Process(T x)
	{
		T y = coef.b0 * x + s1;
		s1 = coef.b1 * x - coef.a1 * y + s2;
		s2 = coef.b2 * x - coef.a2 * y;
		return y;
	}
	// allows inplace (pd==ps)
	void Process(const T* ps, T* pd, size_t l)
	{
		for(size_t i = 0; i < l; ++i) pd[i] = Process(ps[i]);
	}
	void Process(T* p, size_t l)
	{
		Process(p, p, l);
	}
};

using SimpleLPFF = SimpleLPFT<float>;
using SimpleLPFD = SimpleLPFT<double>;
