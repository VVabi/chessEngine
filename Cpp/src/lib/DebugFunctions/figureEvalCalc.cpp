/*
 * evalCalculation.cpp
 *
 *  Created on: Sep 19, 2016
 *      Author: vabi
 */


#include <lib/basics.hpp>
#include <lib/bitfiddling.h>

extern int16_t figureValues[];

int16_t calcFigureEvaluation(chessPosition* position) {
	int16_t val = 0;
	for(uint16_t ind=0; ind < 5; ind++) {
		uint64_t whitePieces = position->pieceTables[white][ind];
		uint64_t blackPieces = position->pieceTables[black][ind];
		val = val+figureValues[ind]*(popcount(whitePieces)-popcount(blackPieces));
	}
	return val;
}




