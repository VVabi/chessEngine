/*
 * pieceTableCalc.cpp
 *
 *  Created on: Sep 19, 2016
 *      Author: vabi
 */

#include <lib/basics.hpp>
#include <lib/bitfiddling.h>

extern int16_t pieceTables[7][2][64];


int16_t calcPieceTableValue(chessPosition* position) {

	int16_t val = 0;
	for(uint16_t ind=0; ind < 6; ind++) {
		uint64_t whitePieces = position->pieceTables[white][ind];
		while(whitePieces){
			uint16_t field = popLSB(whitePieces);
			val = val+pieceTables[ind][white][field];
		}
		uint64_t blackPieces = position->pieceTables[black][ind];
		while(blackPieces){
			uint16_t field = popLSB(blackPieces);
			val = val-pieceTables[ind][black][field];
		}
	}
	return val;
}



