/*
 * pawnEvaluation.cpp
 *
 *  Created on: Sep 25, 2016
 *      Author: vabi
 */


#include <lib/basics.hpp>
#include "evaluation.hpp"
#include <lib/bitfiddling.h>


extern uint64_t files[];
extern uint64_t  passedPawnMasks[2][64];

static short const passedPawnEvalValues[2][64] = {{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 9, 9, 9, 9, 9, 9, 9, 9, 13, 13, 13, 13, 13, 13, 13, 13,0,0,0,0,0,0,0,0 },
{ 0, 0, 0, 0, 0, 0, 0, 0, 13, 13, 13, 13, 13, 13, 13, 13, 9, 9, 9, 9, 9, 9, 9, 9, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1,0,0,0,0,0,0,0,0 }};



int32_t passedPawnEval(uint64_t whitePawns, uint64_t blackPawns) {

	int32_t eval = 0;
	uint64_t whitePawnBuffer = whitePawns;
	while(whitePawnBuffer){
		uint16_t field = popLSB(whitePawnBuffer);
		if((passedPawnMasks[white][field] & blackPawns) == 0) {
			eval = eval+passedPawnEvalValues[white][field];
		}
	}
	uint64_t blackPawnBuffer = blackPawns;
	while(blackPawnBuffer){
		uint16_t field = popLSB(blackPawnBuffer);
		if((passedPawnMasks[black][field] & whitePawns) == 0) {
			eval = eval-passedPawnEvalValues[black][field];
		}
	}


	return 7*eval;

}

int32_t staticPawnEval(uint64_t pawns, playerColor color, uint8_t* pawnColumnOccupancy) { //all stuff depending only on own pawn structure, not the opponents


	int32_t eval = 0;
	*pawnColumnOccupancy = 0;
	uint8_t doublePawns         = 0; //TODO: extend to triple pawns


	for (uint16_t ind=0; ind <8; ind++) {
		uint64_t occ = pawns & files[ind];
		if(occ) {
			*pawnColumnOccupancy = *pawnColumnOccupancy | (1 << ind);
		}
		if(popcount(occ) > 1){
			doublePawns = doublePawns | (1 << ind);
		}
	}


	uint8_t isolatedPawns = ~((*pawnColumnOccupancy << 1) | (*pawnColumnOccupancy >> 1));

	uint64_t isolatedDoublePawns = isolatedPawns & doublePawns;

	uint64_t nonIsolatedDoublePawns =  (~isolatedPawns) & doublePawns;

	eval = eval-50*popcount(isolatedDoublePawns)-20*popcount(nonIsolatedDoublePawns);

	eval = eval-10*popcount(isolatedPawns);

	return eval*(1-2*color);
}


int32_t pawnEvaluation(const chessPosition* position, uint8_t* pawnColumnOccupancy) {

	uint32_t eval=0;
	uint64_t whitePawns = position->pieceTables[white][pawn];
	uint64_t blackPawns = position->pieceTables[black][pawn];


	eval = eval+staticPawnEval(whitePawns, white, pawnColumnOccupancy)+staticPawnEval(blackPawns, black,  pawnColumnOccupancy+1);
	eval = eval+passedPawnEval(whitePawns, blackPawns);
	return eval;
}
