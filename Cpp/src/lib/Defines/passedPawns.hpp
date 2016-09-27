/*
 * passedPawns.hpp
 *
 *  Created on: Sep 25, 2016
 *      Author: vabi
 */

#ifndef LIB_DEFINES_PASSEDPAWNS_HPP_
#define LIB_DEFINES_PASSEDPAWNS_HPP_
#include <stdint.h>

#define WHITEPASSEDPAWNMASK8 (1UL << 16)| (1UL << 24)| (1UL << 32)| (1UL << 40)| (1UL << 48)| (1UL << 17)| (1UL << 25)| (1UL << 33)| (1UL << 41)| (1UL << 49)
#define WHITEPASSEDPAWNMASK9 (1UL << 16)| (1UL << 24)| (1UL << 32)| (1UL << 40)| (1UL << 48)| (1UL << 17)| (1UL << 25)| (1UL << 33)| (1UL << 41)| (1UL << 49)| (1UL << 18)| (1UL << 26)| (1UL << 34)| (1UL << 42)| (1UL << 50)
#define WHITEPASSEDPAWNMASK10 (1UL << 17)| (1UL << 25)| (1UL << 33)| (1UL << 41)| (1UL << 49)| (1UL << 18)| (1UL << 26)| (1UL << 34)| (1UL << 42)| (1UL << 50)| (1UL << 19)| (1UL << 27)| (1UL << 35)| (1UL << 43)| (1UL << 51)
#define WHITEPASSEDPAWNMASK11 (1UL << 18)| (1UL << 26)| (1UL << 34)| (1UL << 42)| (1UL << 50)| (1UL << 19)| (1UL << 27)| (1UL << 35)| (1UL << 43)| (1UL << 51)| (1UL << 20)| (1UL << 28)| (1UL << 36)| (1UL << 44)| (1UL << 52)
#define WHITEPASSEDPAWNMASK12 (1UL << 19)| (1UL << 27)| (1UL << 35)| (1UL << 43)| (1UL << 51)| (1UL << 20)| (1UL << 28)| (1UL << 36)| (1UL << 44)| (1UL << 52)| (1UL << 21)| (1UL << 29)| (1UL << 37)| (1UL << 45)| (1UL << 53)
#define WHITEPASSEDPAWNMASK13 (1UL << 20)| (1UL << 28)| (1UL << 36)| (1UL << 44)| (1UL << 52)| (1UL << 21)| (1UL << 29)| (1UL << 37)| (1UL << 45)| (1UL << 53)| (1UL << 22)| (1UL << 30)| (1UL << 38)| (1UL << 46)| (1UL << 54)
#define WHITEPASSEDPAWNMASK14 (1UL << 21)| (1UL << 29)| (1UL << 37)| (1UL << 45)| (1UL << 53)| (1UL << 22)| (1UL << 30)| (1UL << 38)| (1UL << 46)| (1UL << 54)| (1UL << 23)| (1UL << 31)| (1UL << 39)| (1UL << 47)| (1UL << 55)
#define WHITEPASSEDPAWNMASK15 (1UL << 22)| (1UL << 30)| (1UL << 38)| (1UL << 46)| (1UL << 54)| (1UL << 23)| (1UL << 31)| (1UL << 39)| (1UL << 47)| (1UL << 55)
#define WHITEPASSEDPAWNMASK16 (1UL << 24)| (1UL << 32)| (1UL << 40)| (1UL << 48)| (1UL << 25)| (1UL << 33)| (1UL << 41)| (1UL << 49)
#define WHITEPASSEDPAWNMASK17 (1UL << 24)| (1UL << 32)| (1UL << 40)| (1UL << 48)| (1UL << 25)| (1UL << 33)| (1UL << 41)| (1UL << 49)| (1UL << 26)| (1UL << 34)| (1UL << 42)| (1UL << 50)
#define WHITEPASSEDPAWNMASK18 (1UL << 25)| (1UL << 33)| (1UL << 41)| (1UL << 49)| (1UL << 26)| (1UL << 34)| (1UL << 42)| (1UL << 50)| (1UL << 27)| (1UL << 35)| (1UL << 43)| (1UL << 51)
#define WHITEPASSEDPAWNMASK19 (1UL << 26)| (1UL << 34)| (1UL << 42)| (1UL << 50)| (1UL << 27)| (1UL << 35)| (1UL << 43)| (1UL << 51)| (1UL << 28)| (1UL << 36)| (1UL << 44)| (1UL << 52)
#define WHITEPASSEDPAWNMASK20 (1UL << 27)| (1UL << 35)| (1UL << 43)| (1UL << 51)| (1UL << 28)| (1UL << 36)| (1UL << 44)| (1UL << 52)| (1UL << 29)| (1UL << 37)| (1UL << 45)| (1UL << 53)
#define WHITEPASSEDPAWNMASK21 (1UL << 28)| (1UL << 36)| (1UL << 44)| (1UL << 52)| (1UL << 29)| (1UL << 37)| (1UL << 45)| (1UL << 53)| (1UL << 30)| (1UL << 38)| (1UL << 46)| (1UL << 54)
#define WHITEPASSEDPAWNMASK22 (1UL << 29)| (1UL << 37)| (1UL << 45)| (1UL << 53)| (1UL << 30)| (1UL << 38)| (1UL << 46)| (1UL << 54)| (1UL << 31)| (1UL << 39)| (1UL << 47)| (1UL << 55)
#define WHITEPASSEDPAWNMASK23 (1UL << 30)| (1UL << 38)| (1UL << 46)| (1UL << 54)| (1UL << 31)| (1UL << 39)| (1UL << 47)| (1UL << 55)
#define WHITEPASSEDPAWNMASK24 (1UL << 32)| (1UL << 40)| (1UL << 48)| (1UL << 33)| (1UL << 41)| (1UL << 49)
#define WHITEPASSEDPAWNMASK25 (1UL << 32)| (1UL << 40)| (1UL << 48)| (1UL << 33)| (1UL << 41)| (1UL << 49)| (1UL << 34)| (1UL << 42)| (1UL << 50)
#define WHITEPASSEDPAWNMASK26 (1UL << 33)| (1UL << 41)| (1UL << 49)| (1UL << 34)| (1UL << 42)| (1UL << 50)| (1UL << 35)| (1UL << 43)| (1UL << 51)
#define WHITEPASSEDPAWNMASK27 (1UL << 34)| (1UL << 42)| (1UL << 50)| (1UL << 35)| (1UL << 43)| (1UL << 51)| (1UL << 36)| (1UL << 44)| (1UL << 52)
#define WHITEPASSEDPAWNMASK28 (1UL << 35)| (1UL << 43)| (1UL << 51)| (1UL << 36)| (1UL << 44)| (1UL << 52)| (1UL << 37)| (1UL << 45)| (1UL << 53)
#define WHITEPASSEDPAWNMASK29 (1UL << 36)| (1UL << 44)| (1UL << 52)| (1UL << 37)| (1UL << 45)| (1UL << 53)| (1UL << 38)| (1UL << 46)| (1UL << 54)
#define WHITEPASSEDPAWNMASK30 (1UL << 37)| (1UL << 45)| (1UL << 53)| (1UL << 38)| (1UL << 46)| (1UL << 54)| (1UL << 39)| (1UL << 47)| (1UL << 55)
#define WHITEPASSEDPAWNMASK31 (1UL << 38)| (1UL << 46)| (1UL << 54)| (1UL << 39)| (1UL << 47)| (1UL << 55)
#define WHITEPASSEDPAWNMASK32 (1UL << 40)| (1UL << 48)| (1UL << 41)| (1UL << 49)
#define WHITEPASSEDPAWNMASK33 (1UL << 40)| (1UL << 48)| (1UL << 41)| (1UL << 49)| (1UL << 42)| (1UL << 50)
#define WHITEPASSEDPAWNMASK34 (1UL << 41)| (1UL << 49)| (1UL << 42)| (1UL << 50)| (1UL << 43)| (1UL << 51)
#define WHITEPASSEDPAWNMASK35 (1UL << 42)| (1UL << 50)| (1UL << 43)| (1UL << 51)| (1UL << 44)| (1UL << 52)
#define WHITEPASSEDPAWNMASK36 (1UL << 43)| (1UL << 51)| (1UL << 44)| (1UL << 52)| (1UL << 45)| (1UL << 53)
#define WHITEPASSEDPAWNMASK37 (1UL << 44)| (1UL << 52)| (1UL << 45)| (1UL << 53)| (1UL << 46)| (1UL << 54)
#define WHITEPASSEDPAWNMASK38 (1UL << 45)| (1UL << 53)| (1UL << 46)| (1UL << 54)| (1UL << 47)| (1UL << 55)
#define WHITEPASSEDPAWNMASK39 (1UL << 46)| (1UL << 54)| (1UL << 47)| (1UL << 55)
#define WHITEPASSEDPAWNMASK40 (1UL << 48)| (1UL << 49)
#define WHITEPASSEDPAWNMASK41 (1UL << 48)| (1UL << 49)| (1UL << 50)
#define WHITEPASSEDPAWNMASK42 (1UL << 49)| (1UL << 50)| (1UL << 51)
#define WHITEPASSEDPAWNMASK43 (1UL << 50)| (1UL << 51)| (1UL << 52)
#define WHITEPASSEDPAWNMASK44 (1UL << 51)| (1UL << 52)| (1UL << 53)
#define WHITEPASSEDPAWNMASK45 (1UL << 52)| (1UL << 53)| (1UL << 54)
#define WHITEPASSEDPAWNMASK46 (1UL << 53)| (1UL << 54)| (1UL << 55)
#define WHITEPASSEDPAWNMASK47 (1UL << 54)| (1UL << 55)
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
#define BLACKPASSEDPAWNMASK16 (1UL << 8)| (1UL << 9)
#define BLACKPASSEDPAWNMASK17 (1UL << 8)| (1UL << 9)| (1UL << 10)
#define BLACKPASSEDPAWNMASK18 (1UL << 9)| (1UL << 10)| (1UL << 11)
#define BLACKPASSEDPAWNMASK19 (1UL << 10)| (1UL << 11)| (1UL << 12)
#define BLACKPASSEDPAWNMASK20 (1UL << 11)| (1UL << 12)| (1UL << 13)
#define BLACKPASSEDPAWNMASK21 (1UL << 12)| (1UL << 13)| (1UL << 14)
#define BLACKPASSEDPAWNMASK22 (1UL << 13)| (1UL << 14)| (1UL << 15)
#define BLACKPASSEDPAWNMASK23 (1UL << 14)| (1UL << 15)
#define BLACKPASSEDPAWNMASK24 (1UL << 8)| (1UL << 16)| (1UL << 9)| (1UL << 17)
#define BLACKPASSEDPAWNMASK25 (1UL << 8)| (1UL << 16)| (1UL << 9)| (1UL << 17)| (1UL << 10)| (1UL << 18)
#define BLACKPASSEDPAWNMASK26 (1UL << 9)| (1UL << 17)| (1UL << 10)| (1UL << 18)| (1UL << 11)| (1UL << 19)
#define BLACKPASSEDPAWNMASK27 (1UL << 10)| (1UL << 18)| (1UL << 11)| (1UL << 19)| (1UL << 12)| (1UL << 20)
#define BLACKPASSEDPAWNMASK28 (1UL << 11)| (1UL << 19)| (1UL << 12)| (1UL << 20)| (1UL << 13)| (1UL << 21)
#define BLACKPASSEDPAWNMASK29 (1UL << 12)| (1UL << 20)| (1UL << 13)| (1UL << 21)| (1UL << 14)| (1UL << 22)
#define BLACKPASSEDPAWNMASK30 (1UL << 13)| (1UL << 21)| (1UL << 14)| (1UL << 22)| (1UL << 15)| (1UL << 23)
#define BLACKPASSEDPAWNMASK31 (1UL << 14)| (1UL << 22)| (1UL << 15)| (1UL << 23)
#define BLACKPASSEDPAWNMASK32 (1UL << 8)| (1UL << 16)| (1UL << 24)| (1UL << 9)| (1UL << 17)| (1UL << 25)
#define BLACKPASSEDPAWNMASK33 (1UL << 8)| (1UL << 16)| (1UL << 24)| (1UL << 9)| (1UL << 17)| (1UL << 25)| (1UL << 10)| (1UL << 18)| (1UL << 26)
#define BLACKPASSEDPAWNMASK34 (1UL << 9)| (1UL << 17)| (1UL << 25)| (1UL << 10)| (1UL << 18)| (1UL << 26)| (1UL << 11)| (1UL << 19)| (1UL << 27)
#define BLACKPASSEDPAWNMASK35 (1UL << 10)| (1UL << 18)| (1UL << 26)| (1UL << 11)| (1UL << 19)| (1UL << 27)| (1UL << 12)| (1UL << 20)| (1UL << 28)
#define BLACKPASSEDPAWNMASK36 (1UL << 11)| (1UL << 19)| (1UL << 27)| (1UL << 12)| (1UL << 20)| (1UL << 28)| (1UL << 13)| (1UL << 21)| (1UL << 29)
#define BLACKPASSEDPAWNMASK37 (1UL << 12)| (1UL << 20)| (1UL << 28)| (1UL << 13)| (1UL << 21)| (1UL << 29)| (1UL << 14)| (1UL << 22)| (1UL << 30)
#define BLACKPASSEDPAWNMASK38 (1UL << 13)| (1UL << 21)| (1UL << 29)| (1UL << 14)| (1UL << 22)| (1UL << 30)| (1UL << 15)| (1UL << 23)| (1UL << 31)
#define BLACKPASSEDPAWNMASK39 (1UL << 14)| (1UL << 22)| (1UL << 30)| (1UL << 15)| (1UL << 23)| (1UL << 31)
#define BLACKPASSEDPAWNMASK40 (1UL << 8)| (1UL << 16)| (1UL << 24)| (1UL << 32)| (1UL << 9)| (1UL << 17)| (1UL << 25)| (1UL << 33)
#define BLACKPASSEDPAWNMASK41 (1UL << 8)| (1UL << 16)| (1UL << 24)| (1UL << 32)| (1UL << 9)| (1UL << 17)| (1UL << 25)| (1UL << 33)| (1UL << 10)| (1UL << 18)| (1UL << 26)| (1UL << 34)
#define BLACKPASSEDPAWNMASK42 (1UL << 9)| (1UL << 17)| (1UL << 25)| (1UL << 33)| (1UL << 10)| (1UL << 18)| (1UL << 26)| (1UL << 34)| (1UL << 11)| (1UL << 19)| (1UL << 27)| (1UL << 35)
#define BLACKPASSEDPAWNMASK43 (1UL << 10)| (1UL << 18)| (1UL << 26)| (1UL << 34)| (1UL << 11)| (1UL << 19)| (1UL << 27)| (1UL << 35)| (1UL << 12)| (1UL << 20)| (1UL << 28)| (1UL << 36)
#define BLACKPASSEDPAWNMASK44 (1UL << 11)| (1UL << 19)| (1UL << 27)| (1UL << 35)| (1UL << 12)| (1UL << 20)| (1UL << 28)| (1UL << 36)| (1UL << 13)| (1UL << 21)| (1UL << 29)| (1UL << 37)
#define BLACKPASSEDPAWNMASK45 (1UL << 12)| (1UL << 20)| (1UL << 28)| (1UL << 36)| (1UL << 13)| (1UL << 21)| (1UL << 29)| (1UL << 37)| (1UL << 14)| (1UL << 22)| (1UL << 30)| (1UL << 38)
#define BLACKPASSEDPAWNMASK46 (1UL << 13)| (1UL << 21)| (1UL << 29)| (1UL << 37)| (1UL << 14)| (1UL << 22)| (1UL << 30)| (1UL << 38)| (1UL << 15)| (1UL << 23)| (1UL << 31)| (1UL << 39)
#define BLACKPASSEDPAWNMASK47 (1UL << 14)| (1UL << 22)| (1UL << 30)| (1UL << 38)| (1UL << 15)| (1UL << 23)| (1UL << 31)| (1UL << 39)
#define BLACKPASSEDPAWNMASK48 (1UL << 8)| (1UL << 16)| (1UL << 24)| (1UL << 32)| (1UL << 40)| (1UL << 9)| (1UL << 17)| (1UL << 25)| (1UL << 33)| (1UL << 41)
#define BLACKPASSEDPAWNMASK49 (1UL << 8)| (1UL << 16)| (1UL << 24)| (1UL << 32)| (1UL << 40)| (1UL << 9)| (1UL << 17)| (1UL << 25)| (1UL << 33)| (1UL << 41)| (1UL << 10)| (1UL << 18)| (1UL << 26)| (1UL << 34)| (1UL << 42)
#define BLACKPASSEDPAWNMASK50 (1UL << 9)| (1UL << 17)| (1UL << 25)| (1UL << 33)| (1UL << 41)| (1UL << 10)| (1UL << 18)| (1UL << 26)| (1UL << 34)| (1UL << 42)| (1UL << 11)| (1UL << 19)| (1UL << 27)| (1UL << 35)| (1UL << 43)
#define BLACKPASSEDPAWNMASK51 (1UL << 10)| (1UL << 18)| (1UL << 26)| (1UL << 34)| (1UL << 42)| (1UL << 11)| (1UL << 19)| (1UL << 27)| (1UL << 35)| (1UL << 43)| (1UL << 12)| (1UL << 20)| (1UL << 28)| (1UL << 36)| (1UL << 44)
#define BLACKPASSEDPAWNMASK52 (1UL << 11)| (1UL << 19)| (1UL << 27)| (1UL << 35)| (1UL << 43)| (1UL << 12)| (1UL << 20)| (1UL << 28)| (1UL << 36)| (1UL << 44)| (1UL << 13)| (1UL << 21)| (1UL << 29)| (1UL << 37)| (1UL << 45)
#define BLACKPASSEDPAWNMASK53 (1UL << 12)| (1UL << 20)| (1UL << 28)| (1UL << 36)| (1UL << 44)| (1UL << 13)| (1UL << 21)| (1UL << 29)| (1UL << 37)| (1UL << 45)| (1UL << 14)| (1UL << 22)| (1UL << 30)| (1UL << 38)| (1UL << 46)
#define BLACKPASSEDPAWNMASK54 (1UL << 13)| (1UL << 21)| (1UL << 29)| (1UL << 37)| (1UL << 45)| (1UL << 14)| (1UL << 22)| (1UL << 30)| (1UL << 38)| (1UL << 46)| (1UL << 15)| (1UL << 23)| (1UL << 31)| (1UL << 39)| (1UL << 47)
#define BLACKPASSEDPAWNMASK55 (1UL << 14)| (1UL << 22)| (1UL << 30)| (1UL << 38)| (1UL << 46)| (1UL << 15)| (1UL << 23)| (1UL << 31)| (1UL << 39)| (1UL << 47)


#endif /* LIB_DEFINES_PASSEDPAWNS_HPP_ */
