//
//  FloatGrain.h
//  generic implementation
//
//  created by yu2924 on 2023-02-24
//

#pragma once

namespace FloatGrain
{

	static inline double round(double v, double grain)
	{
		return (double)(int64_t)(v / grain + ((0 <= v) ? 0.5 : -0.5)) * grain;
	}

	static inline double truncate(double v, double grain)
	{
		return (double)(int64_t)(v / grain) * grain;
	}

} // namespace FloatGrain
