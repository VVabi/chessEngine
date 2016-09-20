/*
 * enPassantMoveTables.cpp
 *
 *  Created on: Sep 20, 2016
 *      Author: vabi
 */



#include <stdint.h>
#include "enPassantTables.hpp"


uint64_t enPassantMoveTable[2][8] = {
		{WHITEFILEA,WHITEFILEB,WHITEFILEC,WHITEFILED,WHITEFILEE,WHITEFILEF,WHITEFILEG,WHITEFILEH},
		{BLACKFILEA,BLACKFILEB,BLACKFILEC,BLACKFILED,BLACKFILEE,BLACKFILEF,BLACKFILEG,BLACKFILEH}
};


uint16_t enPassantTargetFields[2][8] = {
		{40,41,42,43,44,45,46,47},
		{16,17,18,19,20,21,22,23}
};
