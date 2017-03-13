/*
 * helper_func.h
 *
 *  Created on: Jun 9, 2015
 *      Author: fabian
 */

#ifndef BIT_FIDDLING_
#define BIT_FIDDLING_
#include <stdint.h>


#define LOWESTBITONLY(a) (a & (-a))

#define KILLLSB() (a )
inline uint16_t findLSB(const uint64_t a){
	return __builtin_ctzll(a);
}

inline uint16_t findMSB(const uint64_t a){
	return 63-__builtin_clzll(a); //TODO: replace by ctz?

}

inline uint16_t popLSB(uint64_t &a){
	uint16_t n = __builtin_ctzll(a);
	a = a & (~(1ULL << n));
	return n;
}

inline uint16_t popcount(uint64_t x){
	    x = (x & 0x5555555555555555ULL) + ((x >> 1) & 0x5555555555555555ULL);
	    x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
	    x = (x & 0x0F0F0F0F0F0F0F0FULL) + ((x >> 4) & 0x0F0F0F0F0F0F0F0FULL);
	    return (x * 0x0101010101010101ULL) >> 56;
	//return __builtin_popcountll(a);
}



#endif /* HELPER_FUNC_H_ */
