/*
 * passedPawns.cpp
 *
 *  Created on: Sep 25, 2016
 *      Author: vabi
 */



#include "passedPawns.hpp"


uint64_t  passedPawnMasks[2][64] = {
		{0, 0, 0, 0, 0, 0, 0, 0, WHITEPASSEDPAWNMASK8, WHITEPASSEDPAWNMASK9, WHITEPASSEDPAWNMASK10, WHITEPASSEDPAWNMASK11, WHITEPASSEDPAWNMASK12, WHITEPASSEDPAWNMASK13, WHITEPASSEDPAWNMASK14, WHITEPASSEDPAWNMASK15, WHITEPASSEDPAWNMASK16, WHITEPASSEDPAWNMASK17, WHITEPASSEDPAWNMASK18, WHITEPASSEDPAWNMASK19, WHITEPASSEDPAWNMASK20, WHITEPASSEDPAWNMASK21, WHITEPASSEDPAWNMASK22, WHITEPASSEDPAWNMASK23, WHITEPASSEDPAWNMASK24, WHITEPASSEDPAWNMASK25, WHITEPASSEDPAWNMASK26, WHITEPASSEDPAWNMASK27, WHITEPASSEDPAWNMASK28, WHITEPASSEDPAWNMASK29, WHITEPASSEDPAWNMASK30, WHITEPASSEDPAWNMASK31, WHITEPASSEDPAWNMASK32, WHITEPASSEDPAWNMASK33, WHITEPASSEDPAWNMASK34, WHITEPASSEDPAWNMASK35, WHITEPASSEDPAWNMASK36, WHITEPASSEDPAWNMASK37, WHITEPASSEDPAWNMASK38, WHITEPASSEDPAWNMASK39, WHITEPASSEDPAWNMASK40, WHITEPASSEDPAWNMASK41, WHITEPASSEDPAWNMASK42, WHITEPASSEDPAWNMASK43, WHITEPASSEDPAWNMASK44, WHITEPASSEDPAWNMASK45, WHITEPASSEDPAWNMASK46, WHITEPASSEDPAWNMASK47, WHITEPASSEDPAWNMASK48, WHITEPASSEDPAWNMASK49, WHITEPASSEDPAWNMASK50, WHITEPASSEDPAWNMASK51, WHITEPASSEDPAWNMASK52, WHITEPASSEDPAWNMASK53, WHITEPASSEDPAWNMASK54, WHITEPASSEDPAWNMASK55 },
		{0,0,0,0,0,0,0,0,BLACKPASSEDPAWNMASK8, BLACKPASSEDPAWNMASK9, BLACKPASSEDPAWNMASK10, BLACKPASSEDPAWNMASK11, BLACKPASSEDPAWNMASK12, BLACKPASSEDPAWNMASK13, BLACKPASSEDPAWNMASK14, BLACKPASSEDPAWNMASK15, BLACKPASSEDPAWNMASK16, BLACKPASSEDPAWNMASK17, BLACKPASSEDPAWNMASK18, BLACKPASSEDPAWNMASK19, BLACKPASSEDPAWNMASK20, BLACKPASSEDPAWNMASK21, BLACKPASSEDPAWNMASK22, BLACKPASSEDPAWNMASK23, BLACKPASSEDPAWNMASK24, BLACKPASSEDPAWNMASK25, BLACKPASSEDPAWNMASK26, BLACKPASSEDPAWNMASK27, BLACKPASSEDPAWNMASK28, BLACKPASSEDPAWNMASK29, BLACKPASSEDPAWNMASK30, BLACKPASSEDPAWNMASK31, BLACKPASSEDPAWNMASK32, BLACKPASSEDPAWNMASK33, BLACKPASSEDPAWNMASK34, BLACKPASSEDPAWNMASK35, BLACKPASSEDPAWNMASK36, BLACKPASSEDPAWNMASK37, BLACKPASSEDPAWNMASK38, BLACKPASSEDPAWNMASK39, BLACKPASSEDPAWNMASK40, BLACKPASSEDPAWNMASK41, BLACKPASSEDPAWNMASK42, BLACKPASSEDPAWNMASK43, BLACKPASSEDPAWNMASK44, BLACKPASSEDPAWNMASK45, BLACKPASSEDPAWNMASK46, BLACKPASSEDPAWNMASK47, BLACKPASSEDPAWNMASK48, BLACKPASSEDPAWNMASK49, BLACKPASSEDPAWNMASK50, BLACKPASSEDPAWNMASK51, BLACKPASSEDPAWNMASK52, BLACKPASSEDPAWNMASK53, BLACKPASSEDPAWNMASK54, BLACKPASSEDPAWNMASK55}
};

