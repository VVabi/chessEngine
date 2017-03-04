/*
 * passedPawns.hpp
 *
 *  Created on: Sep 25, 2016
 *      Author: vabi
 */

#ifndef LIB_DEFINES_PASSEDPAWNS_HPP_
#define LIB_DEFINES_PASSEDPAWNS_HPP_
#include <stdint.h>

#define WHITEPASSEDPAWNMASK8 (1ULL << 16)| (1ULL << 24)| (1ULL << 32)| (1ULL << 40)| (1ULL << 48)| (1ULL << 17)| (1ULL << 25)| (1ULL << 33)| (1ULL << 41)| (1ULL << 49)
#define WHITEPASSEDPAWNMASK9 (1ULL << 16)| (1ULL << 24)| (1ULL << 32)| (1ULL << 40)| (1ULL << 48)| (1ULL << 17)| (1ULL << 25)| (1ULL << 33)| (1ULL << 41)| (1ULL << 49)| (1ULL << 18)| (1ULL << 26)| (1ULL << 34)| (1ULL << 42)| (1ULL << 50)
#define WHITEPASSEDPAWNMASK10 (1ULL << 17)| (1ULL << 25)| (1ULL << 33)| (1ULL << 41)| (1ULL << 49)| (1ULL << 18)| (1ULL << 26)| (1ULL << 34)| (1ULL << 42)| (1ULL << 50)| (1ULL << 19)| (1ULL << 27)| (1ULL << 35)| (1ULL << 43)| (1ULL << 51)
#define WHITEPASSEDPAWNMASK11 (1ULL << 18)| (1ULL << 26)| (1ULL << 34)| (1ULL << 42)| (1ULL << 50)| (1ULL << 19)| (1ULL << 27)| (1ULL << 35)| (1ULL << 43)| (1ULL << 51)| (1ULL << 20)| (1ULL << 28)| (1ULL << 36)| (1ULL << 44)| (1ULL << 52)
#define WHITEPASSEDPAWNMASK12 (1ULL << 19)| (1ULL << 27)| (1ULL << 35)| (1ULL << 43)| (1ULL << 51)| (1ULL << 20)| (1ULL << 28)| (1ULL << 36)| (1ULL << 44)| (1ULL << 52)| (1ULL << 21)| (1ULL << 29)| (1ULL << 37)| (1ULL << 45)| (1ULL << 53)
#define WHITEPASSEDPAWNMASK13 (1ULL << 20)| (1ULL << 28)| (1ULL << 36)| (1ULL << 44)| (1ULL << 52)| (1ULL << 21)| (1ULL << 29)| (1ULL << 37)| (1ULL << 45)| (1ULL << 53)| (1ULL << 22)| (1ULL << 30)| (1ULL << 38)| (1ULL << 46)| (1ULL << 54)
#define WHITEPASSEDPAWNMASK14 (1ULL << 21)| (1ULL << 29)| (1ULL << 37)| (1ULL << 45)| (1ULL << 53)| (1ULL << 22)| (1ULL << 30)| (1ULL << 38)| (1ULL << 46)| (1ULL << 54)| (1ULL << 23)| (1ULL << 31)| (1ULL << 39)| (1ULL << 47)| (1ULL << 55)
#define WHITEPASSEDPAWNMASK15 (1ULL << 22)| (1ULL << 30)| (1ULL << 38)| (1ULL << 46)| (1ULL << 54)| (1ULL << 23)| (1ULL << 31)| (1ULL << 39)| (1ULL << 47)| (1ULL << 55)
#define WHITEPASSEDPAWNMASK16 (1ULL << 24)| (1ULL << 32)| (1ULL << 40)| (1ULL << 48)| (1ULL << 25)| (1ULL << 33)| (1ULL << 41)| (1ULL << 49)
#define WHITEPASSEDPAWNMASK17 (1ULL << 24)| (1ULL << 32)| (1ULL << 40)| (1ULL << 48)| (1ULL << 25)| (1ULL << 33)| (1ULL << 41)| (1ULL << 49)| (1ULL << 26)| (1ULL << 34)| (1ULL << 42)| (1ULL << 50)
#define WHITEPASSEDPAWNMASK18 (1ULL << 25)| (1ULL << 33)| (1ULL << 41)| (1ULL << 49)| (1ULL << 26)| (1ULL << 34)| (1ULL << 42)| (1ULL << 50)| (1ULL << 27)| (1ULL << 35)| (1ULL << 43)| (1ULL << 51)
#define WHITEPASSEDPAWNMASK19 (1ULL << 26)| (1ULL << 34)| (1ULL << 42)| (1ULL << 50)| (1ULL << 27)| (1ULL << 35)| (1ULL << 43)| (1ULL << 51)| (1ULL << 28)| (1ULL << 36)| (1ULL << 44)| (1ULL << 52)
#define WHITEPASSEDPAWNMASK20 (1ULL << 27)| (1ULL << 35)| (1ULL << 43)| (1ULL << 51)| (1ULL << 28)| (1ULL << 36)| (1ULL << 44)| (1ULL << 52)| (1ULL << 29)| (1ULL << 37)| (1ULL << 45)| (1ULL << 53)
#define WHITEPASSEDPAWNMASK21 (1ULL << 28)| (1ULL << 36)| (1ULL << 44)| (1ULL << 52)| (1ULL << 29)| (1ULL << 37)| (1ULL << 45)| (1ULL << 53)| (1ULL << 30)| (1ULL << 38)| (1ULL << 46)| (1ULL << 54)
#define WHITEPASSEDPAWNMASK22 (1ULL << 29)| (1ULL << 37)| (1ULL << 45)| (1ULL << 53)| (1ULL << 30)| (1ULL << 38)| (1ULL << 46)| (1ULL << 54)| (1ULL << 31)| (1ULL << 39)| (1ULL << 47)| (1ULL << 55)
#define WHITEPASSEDPAWNMASK23 (1ULL << 30)| (1ULL << 38)| (1ULL << 46)| (1ULL << 54)| (1ULL << 31)| (1ULL << 39)| (1ULL << 47)| (1ULL << 55)
#define WHITEPASSEDPAWNMASK24 (1ULL << 32)| (1ULL << 40)| (1ULL << 48)| (1ULL << 33)| (1ULL << 41)| (1ULL << 49)
#define WHITEPASSEDPAWNMASK25 (1ULL << 32)| (1ULL << 40)| (1ULL << 48)| (1ULL << 33)| (1ULL << 41)| (1ULL << 49)| (1ULL << 34)| (1ULL << 42)| (1ULL << 50)
#define WHITEPASSEDPAWNMASK26 (1ULL << 33)| (1ULL << 41)| (1ULL << 49)| (1ULL << 34)| (1ULL << 42)| (1ULL << 50)| (1ULL << 35)| (1ULL << 43)| (1ULL << 51)
#define WHITEPASSEDPAWNMASK27 (1ULL << 34)| (1ULL << 42)| (1ULL << 50)| (1ULL << 35)| (1ULL << 43)| (1ULL << 51)| (1ULL << 36)| (1ULL << 44)| (1ULL << 52)
#define WHITEPASSEDPAWNMASK28 (1ULL << 35)| (1ULL << 43)| (1ULL << 51)| (1ULL << 36)| (1ULL << 44)| (1ULL << 52)| (1ULL << 37)| (1ULL << 45)| (1ULL << 53)
#define WHITEPASSEDPAWNMASK29 (1ULL << 36)| (1ULL << 44)| (1ULL << 52)| (1ULL << 37)| (1ULL << 45)| (1ULL << 53)| (1ULL << 38)| (1ULL << 46)| (1ULL << 54)
#define WHITEPASSEDPAWNMASK30 (1ULL << 37)| (1ULL << 45)| (1ULL << 53)| (1ULL << 38)| (1ULL << 46)| (1ULL << 54)| (1ULL << 39)| (1ULL << 47)| (1ULL << 55)
#define WHITEPASSEDPAWNMASK31 (1ULL << 38)| (1ULL << 46)| (1ULL << 54)| (1ULL << 39)| (1ULL << 47)| (1ULL << 55)
#define WHITEPASSEDPAWNMASK32 (1ULL << 40)| (1ULL << 48)| (1ULL << 41)| (1ULL << 49)
#define WHITEPASSEDPAWNMASK33 (1ULL << 40)| (1ULL << 48)| (1ULL << 41)| (1ULL << 49)| (1ULL << 42)| (1ULL << 50)
#define WHITEPASSEDPAWNMASK34 (1ULL << 41)| (1ULL << 49)| (1ULL << 42)| (1ULL << 50)| (1ULL << 43)| (1ULL << 51)
#define WHITEPASSEDPAWNMASK35 (1ULL << 42)| (1ULL << 50)| (1ULL << 43)| (1ULL << 51)| (1ULL << 44)| (1ULL << 52)
#define WHITEPASSEDPAWNMASK36 (1ULL << 43)| (1ULL << 51)| (1ULL << 44)| (1ULL << 52)| (1ULL << 45)| (1ULL << 53)
#define WHITEPASSEDPAWNMASK37 (1ULL << 44)| (1ULL << 52)| (1ULL << 45)| (1ULL << 53)| (1ULL << 46)| (1ULL << 54)
#define WHITEPASSEDPAWNMASK38 (1ULL << 45)| (1ULL << 53)| (1ULL << 46)| (1ULL << 54)| (1ULL << 47)| (1ULL << 55)
#define WHITEPASSEDPAWNMASK39 (1ULL << 46)| (1ULL << 54)| (1ULL << 47)| (1ULL << 55)
#define WHITEPASSEDPAWNMASK40 (1ULL << 48)| (1ULL << 49)
#define WHITEPASSEDPAWNMASK41 (1ULL << 48)| (1ULL << 49)| (1ULL << 50)
#define WHITEPASSEDPAWNMASK42 (1ULL << 49)| (1ULL << 50)| (1ULL << 51)
#define WHITEPASSEDPAWNMASK43 (1ULL << 50)| (1ULL << 51)| (1ULL << 52)
#define WHITEPASSEDPAWNMASK44 (1ULL << 51)| (1ULL << 52)| (1ULL << 53)
#define WHITEPASSEDPAWNMASK45 (1ULL << 52)| (1ULL << 53)| (1ULL << 54)
#define WHITEPASSEDPAWNMASK46 (1ULL << 53)| (1ULL << 54)| (1ULL << 55)
#define WHITEPASSEDPAWNMASK47 (1ULL << 54)| (1ULL << 55)
#define WHITEPASSEDPAWNMASK48 0
#define WHITEPASSEDPAWNMASK49 0
#define WHITEPASSEDPAWNMASK50 0
#define WHITEPASSEDPAWNMASK51 0
#define WHITEPASSEDPAWNMASK52 0
#define WHITEPASSEDPAWNMASK53 0
#define WHITEPASSEDPAWNMASK54 0
#define WHITEPASSEDPAWNMASK55 0
#define BLACKPASSEDPAWNMASK8 0
#define BLACKPASSEDPAWNMASK9 0
#define BLACKPASSEDPAWNMASK10 0
#define BLACKPASSEDPAWNMASK11 0
#define BLACKPASSEDPAWNMASK12 0
#define BLACKPASSEDPAWNMASK13 0
#define BLACKPASSEDPAWNMASK14 0
#define BLACKPASSEDPAWNMASK15 0
#define BLACKPASSEDPAWNMASK16 (1ULL << 8)| (1ULL << 9)
#define BLACKPASSEDPAWNMASK17 (1ULL << 8)| (1ULL << 9)| (1ULL << 10)
#define BLACKPASSEDPAWNMASK18 (1ULL << 9)| (1ULL << 10)| (1ULL << 11)
#define BLACKPASSEDPAWNMASK19 (1ULL << 10)| (1ULL << 11)| (1ULL << 12)
#define BLACKPASSEDPAWNMASK20 (1ULL << 11)| (1ULL << 12)| (1ULL << 13)
#define BLACKPASSEDPAWNMASK21 (1ULL << 12)| (1ULL << 13)| (1ULL << 14)
#define BLACKPASSEDPAWNMASK22 (1ULL << 13)| (1ULL << 14)| (1ULL << 15)
#define BLACKPASSEDPAWNMASK23 (1ULL << 14)| (1ULL << 15)
#define BLACKPASSEDPAWNMASK24 (1ULL << 8)| (1ULL << 16)| (1ULL << 9)| (1ULL << 17)
#define BLACKPASSEDPAWNMASK25 (1ULL << 8)| (1ULL << 16)| (1ULL << 9)| (1ULL << 17)| (1ULL << 10)| (1ULL << 18)
#define BLACKPASSEDPAWNMASK26 (1ULL << 9)| (1ULL << 17)| (1ULL << 10)| (1ULL << 18)| (1ULL << 11)| (1ULL << 19)
#define BLACKPASSEDPAWNMASK27 (1ULL << 10)| (1ULL << 18)| (1ULL << 11)| (1ULL << 19)| (1ULL << 12)| (1ULL << 20)
#define BLACKPASSEDPAWNMASK28 (1ULL << 11)| (1ULL << 19)| (1ULL << 12)| (1ULL << 20)| (1ULL << 13)| (1ULL << 21)
#define BLACKPASSEDPAWNMASK29 (1ULL << 12)| (1ULL << 20)| (1ULL << 13)| (1ULL << 21)| (1ULL << 14)| (1ULL << 22)
#define BLACKPASSEDPAWNMASK30 (1ULL << 13)| (1ULL << 21)| (1ULL << 14)| (1ULL << 22)| (1ULL << 15)| (1ULL << 23)
#define BLACKPASSEDPAWNMASK31 (1ULL << 14)| (1ULL << 22)| (1ULL << 15)| (1ULL << 23)
#define BLACKPASSEDPAWNMASK32 (1ULL << 8)| (1ULL << 16)| (1ULL << 24)| (1ULL << 9)| (1ULL << 17)| (1ULL << 25)
#define BLACKPASSEDPAWNMASK33 (1ULL << 8)| (1ULL << 16)| (1ULL << 24)| (1ULL << 9)| (1ULL << 17)| (1ULL << 25)| (1ULL << 10)| (1ULL << 18)| (1ULL << 26)
#define BLACKPASSEDPAWNMASK34 (1ULL << 9)| (1ULL << 17)| (1ULL << 25)| (1ULL << 10)| (1ULL << 18)| (1ULL << 26)| (1ULL << 11)| (1ULL << 19)| (1ULL << 27)
#define BLACKPASSEDPAWNMASK35 (1ULL << 10)| (1ULL << 18)| (1ULL << 26)| (1ULL << 11)| (1ULL << 19)| (1ULL << 27)| (1ULL << 12)| (1ULL << 20)| (1ULL << 28)
#define BLACKPASSEDPAWNMASK36 (1ULL << 11)| (1ULL << 19)| (1ULL << 27)| (1ULL << 12)| (1ULL << 20)| (1ULL << 28)| (1ULL << 13)| (1ULL << 21)| (1ULL << 29)
#define BLACKPASSEDPAWNMASK37 (1ULL << 12)| (1ULL << 20)| (1ULL << 28)| (1ULL << 13)| (1ULL << 21)| (1ULL << 29)| (1ULL << 14)| (1ULL << 22)| (1ULL << 30)
#define BLACKPASSEDPAWNMASK38 (1ULL << 13)| (1ULL << 21)| (1ULL << 29)| (1ULL << 14)| (1ULL << 22)| (1ULL << 30)| (1ULL << 15)| (1ULL << 23)| (1ULL << 31)
#define BLACKPASSEDPAWNMASK39 (1ULL << 14)| (1ULL << 22)| (1ULL << 30)| (1ULL << 15)| (1ULL << 23)| (1ULL << 31)
#define BLACKPASSEDPAWNMASK40 (1ULL << 8)| (1ULL << 16)| (1ULL << 24)| (1ULL << 32)| (1ULL << 9)| (1ULL << 17)| (1ULL << 25)| (1ULL << 33)
#define BLACKPASSEDPAWNMASK41 (1ULL << 8)| (1ULL << 16)| (1ULL << 24)| (1ULL << 32)| (1ULL << 9)| (1ULL << 17)| (1ULL << 25)| (1ULL << 33)| (1ULL << 10)| (1ULL << 18)| (1ULL << 26)| (1ULL << 34)
#define BLACKPASSEDPAWNMASK42 (1ULL << 9)| (1ULL << 17)| (1ULL << 25)| (1ULL << 33)| (1ULL << 10)| (1ULL << 18)| (1ULL << 26)| (1ULL << 34)| (1ULL << 11)| (1ULL << 19)| (1ULL << 27)| (1ULL << 35)
#define BLACKPASSEDPAWNMASK43 (1ULL << 10)| (1ULL << 18)| (1ULL << 26)| (1ULL << 34)| (1ULL << 11)| (1ULL << 19)| (1ULL << 27)| (1ULL << 35)| (1ULL << 12)| (1ULL << 20)| (1ULL << 28)| (1ULL << 36)
#define BLACKPASSEDPAWNMASK44 (1ULL << 11)| (1ULL << 19)| (1ULL << 27)| (1ULL << 35)| (1ULL << 12)| (1ULL << 20)| (1ULL << 28)| (1ULL << 36)| (1ULL << 13)| (1ULL << 21)| (1ULL << 29)| (1ULL << 37)
#define BLACKPASSEDPAWNMASK45 (1ULL << 12)| (1ULL << 20)| (1ULL << 28)| (1ULL << 36)| (1ULL << 13)| (1ULL << 21)| (1ULL << 29)| (1ULL << 37)| (1ULL << 14)| (1ULL << 22)| (1ULL << 30)| (1ULL << 38)
#define BLACKPASSEDPAWNMASK46 (1ULL << 13)| (1ULL << 21)| (1ULL << 29)| (1ULL << 37)| (1ULL << 14)| (1ULL << 22)| (1ULL << 30)| (1ULL << 38)| (1ULL << 15)| (1ULL << 23)| (1ULL << 31)| (1ULL << 39)
#define BLACKPASSEDPAWNMASK47 (1ULL << 14)| (1ULL << 22)| (1ULL << 30)| (1ULL << 38)| (1ULL << 15)| (1ULL << 23)| (1ULL << 31)| (1ULL << 39)
#define BLACKPASSEDPAWNMASK48 (1ULL << 8)| (1ULL << 16)| (1ULL << 24)| (1ULL << 32)| (1ULL << 40)| (1ULL << 9)| (1ULL << 17)| (1ULL << 25)| (1ULL << 33)| (1ULL << 41)
#define BLACKPASSEDPAWNMASK49 (1ULL << 8)| (1ULL << 16)| (1ULL << 24)| (1ULL << 32)| (1ULL << 40)| (1ULL << 9)| (1ULL << 17)| (1ULL << 25)| (1ULL << 33)| (1ULL << 41)| (1ULL << 10)| (1ULL << 18)| (1ULL << 26)| (1ULL << 34)| (1ULL << 42)
#define BLACKPASSEDPAWNMASK50 (1ULL << 9)| (1ULL << 17)| (1ULL << 25)| (1ULL << 33)| (1ULL << 41)| (1ULL << 10)| (1ULL << 18)| (1ULL << 26)| (1ULL << 34)| (1ULL << 42)| (1ULL << 11)| (1ULL << 19)| (1ULL << 27)| (1ULL << 35)| (1ULL << 43)
#define BLACKPASSEDPAWNMASK51 (1ULL << 10)| (1ULL << 18)| (1ULL << 26)| (1ULL << 34)| (1ULL << 42)| (1ULL << 11)| (1ULL << 19)| (1ULL << 27)| (1ULL << 35)| (1ULL << 43)| (1ULL << 12)| (1ULL << 20)| (1ULL << 28)| (1ULL << 36)| (1ULL << 44)
#define BLACKPASSEDPAWNMASK52 (1ULL << 11)| (1ULL << 19)| (1ULL << 27)| (1ULL << 35)| (1ULL << 43)| (1ULL << 12)| (1ULL << 20)| (1ULL << 28)| (1ULL << 36)| (1ULL << 44)| (1ULL << 13)| (1ULL << 21)| (1ULL << 29)| (1ULL << 37)| (1ULL << 45)
#define BLACKPASSEDPAWNMASK53 (1ULL << 12)| (1ULL << 20)| (1ULL << 28)| (1ULL << 36)| (1ULL << 44)| (1ULL << 13)| (1ULL << 21)| (1ULL << 29)| (1ULL << 37)| (1ULL << 45)| (1ULL << 14)| (1ULL << 22)| (1ULL << 30)| (1ULL << 38)| (1ULL << 46)
#define BLACKPASSEDPAWNMASK54 (1ULL << 13)| (1ULL << 21)| (1ULL << 29)| (1ULL << 37)| (1ULL << 45)| (1ULL << 14)| (1ULL << 22)| (1ULL << 30)| (1ULL << 38)| (1ULL << 46)| (1ULL << 15)| (1ULL << 23)| (1ULL << 31)| (1ULL << 39)| (1ULL << 47)
#define BLACKPASSEDPAWNMASK55 (1ULL << 14)| (1ULL << 22)| (1ULL << 30)| (1ULL << 38)| (1ULL << 46)| (1ULL << 15)| (1ULL << 23)| (1ULL << 31)| (1ULL << 39)| (1ULL << 47)


#endif /* LIB_DEFINES_PASSEDPAWNS_HPP_ */
