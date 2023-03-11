//
//  MLS.h
//  generic implementation
//
//  created by yu2924 on 2023-03-07
//

#pragma once

#include <vector>

// 
// MLS (maximum length sequence) using LFSR
// 
// shiftdirection=left, feedback to LSB, output from MSB
//    MSB         LSB
//  <-|n|...|3|2|1|0|<-
// 
struct MLS
{
	uint32_t shiftreg = 0;
	uint32_t bitmask = 0;
	unsigned int numBits = 0;
	std::vector<unsigned int> taps;
	MLS(unsigned int l) { setNumBits(l); }
	unsigned int getNumBits() const { return numBits; }
	void setNumBits(unsigned int l)
	{
		numBits = std::max(3u, std::min(32u, l));
		bitmask = (1 << numBits) - 1;
		switch(numBits)
		{
			// cf. Efficient Shift Registers, LFSR Counters, and Long Pseudo-Random Sequence Generators
			// https://docs.xilinx.com/v/u/en-US/xapp052
			case  3: taps = { 2,1 }; break;
			case  4: taps = { 3,2 }; break;
			case  5: taps = { 4,2 }; break;
			case  6: taps = { 5,4 }; break;
			case  7: taps = { 6,5 }; break;
			case  8: taps = { 7,5,4,3 }; break;
			case  9: taps = { 8,4 }; break;
			case 10: taps = { 9,6 }; break;
			case 11: taps = { 10,8 }; break;
			case 12: taps = { 11,5,3,0 }; break;
			case 13: taps = { 12,3,2,0 }; break;
			case 14: taps = { 13,4,2,0 }; break;
			case 15: taps = { 14,13 }; break;
			case 16: taps = { 15,14,12,3 }; break;
			case 17: taps = { 16,13 }; break;
			case 18: taps = { 17,10 }; break;
			case 19: taps = { 18,5,1,0 }; break;
			case 20: taps = { 19,16 }; break;
			case 21: taps = { 20,18 }; break;
			case 22: taps = { 21,20 }; break;
			case 23: taps = { 22,17 }; break;
			case 24: taps = { 23,22,21,16 }; break;
			case 25: taps = { 24,21 }; break;
			case 26: taps = { 25,5,1,0 }; break;
			case 27: taps = { 26,4,1,0 }; break;
			case 28: taps = { 27,24 }; break;
			case 29: taps = { 28,26 }; break;
			case 30: taps = { 29,5,3,0 }; break;
			case 31: taps = { 30,27 }; break;
			case 32: taps = { 31,21,1,0 }; break;
			default: taps = {}; break;
		}
		reset();
	}
	void reset(uint32_t v)
	{
		v &= bitmask;
		if(!v) v = 1 << (numBits - 1);
		shiftreg = v;
	}
	void reset()
	{
		shiftreg = 1 << (numBits - 1);
	}
	uint8_t get() const
	{
		return shiftreg & (1 << (numBits - 1)) ? 1 : 0;
	}
	uint8_t next()
	{
		uint8_t fb = 0; for(auto t : taps) fb += (shiftreg & (1 << t)) ? 1 : 0;
		shiftreg = ((shiftreg << 1) | (fb & 0x01)) & bitmask;
		return get();
	}
	uint8_t operator()()
	{
		return next();
	}
};
