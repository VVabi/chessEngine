/*
 * helper_func.h
 *
 *  Created on: Jun 9, 2015
 *      Author: fabian
 */

#ifndef BIT_FIDDLING_
#define BIT_FIDDLING_
#include <stdint.h>
#include <lib/Defines/boardParts.hpp>

#define LOWESTBITONLY(a) (a & (-a))

#define KILLLSB() (a )
inline uint16_t findLSB(const uint64_t a) {
    return __builtin_ctzll(a);
}

inline uint16_t findMSB(const uint64_t a) {
    return 63-__builtin_clzll(a); //TODO: replace by ctz?

}

inline uint16_t popLSB(uint64_t &a) {
    uint16_t n = __builtin_ctzll(a);
    a = a & (~(1ULL << n));
    return n;
}

inline uint16_t popcount(uint64_t x) {
        x = (x & 0x5555555555555555ULL) + ((x >> 1) & 0x5555555555555555ULL);
        x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
        x = (x & 0x0F0F0F0F0F0F0F0FULL) + ((x >> 4) & 0x0F0F0F0F0F0F0F0FULL);
        return (x * 0x0101010101010101ULL) >> 56;
    //return __builtin_popcountll(a);
}

#define NORTHONE(x) ((x) << 8)
#define SOUTHONE(x) ((x) >> 8)
#define WESTONE(x) (((x) >> 1) & NOTFILEH)
#define EASTONE(x)  (((x) << 1) & NOTFILEA)


inline uint64_t northFill(uint64_t gen) {
   gen |= (gen <<  8);
   gen |= (gen << 16);
   gen |= (gen << 32);
   return gen;
}

inline uint64_t southFill(uint64_t gen) {
   gen |= (gen >>  8);
   gen |= (gen >> 16);
   gen |= (gen >> 32);
   return gen;
}

inline uint8_t getColumnOcc(uint64_t in) {
    in = southFill(in) | northFill(in);

    return in & 0xFF;


}

#endif /* HELPER_FUNC_H_ */
