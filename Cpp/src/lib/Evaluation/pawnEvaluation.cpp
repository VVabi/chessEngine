/*
 * pawnEvaluation.cpp
 *
 *  Created on: Sep 25, 2016
 *      Author: vabi
 */


#include <lib/basics.hpp>
#include "evaluation.hpp"
#include <lib/bitfiddling.h>
#include <lib/Defines/boardParts.hpp>

extern uint64_t files[];
extern uint64_t  passedPawnMasks[2][64];
extern uint16_t taperingValues[81];

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


int32_t pawnEvaluation(const chessPosition* position, uint8_t* pawnColumnOccupancy, uint16_t phase) {

	uint32_t eval=0;
	uint64_t whitePawns = position->pieceTables[white][pawn];
	uint64_t blackPawns = position->pieceTables[black][pawn];


	eval = eval+staticPawnEval(whitePawns, white, pawnColumnOccupancy)+staticPawnEval(blackPawns, black,  pawnColumnOccupancy+1);
	int32_t passedPawns = passedPawnEval(whitePawns, blackPawns);
	passedPawns = ((256-taperingValues[phase])*passedPawns)/256;
	eval = eval+passedPawns;

/*#ifdef EXPERIMENTAL  looks like very small gain (~ 5-6 elo), but I was hoping for more...
	//backwards pawn
	//int64_t wpawns = position->pieceTables[white][pawn];

	//uint64_t wtakesRight =  wpawns << 9 & NOTFILEA;
	//uint64_t wtakesLeft =   wpawns << 7 & NOTFILEH;
	//uint64_t wtakes     = wtakesLeft | wtakesRight;

	//uint64_t bpawns = position->pieceTables[black][pawn];

	//uint64_t btakesRight =  bpawns >> 7  & NOTFILEA;
	//uint64_t btakesLeft  =  bpawns >> 9  & NOTFILEH;
	//uint64_t btakes      =  btakesLeft | btakesRight;

	uint64_t buffer = position->pieceTables[white][pawn];
	while(buffer){
		uint16_t n = popLSB(buffer)+8;
		if(!(passedPawnMasks[black][n] & (position->pieceTables[white][pawn] & ~(1UL << (n-8)))) && !(position->pieceTables[black][pawn] & files[FILE(n)])){
			eval = eval-15;

			//if(btakes & BIT64(n)){
			//	eval = eval-10;
			//}
		}
	}

	buffer = position->pieceTables[black][pawn];
	while(buffer){
		uint16_t n = popLSB(buffer)-8;
		if(!(passedPawnMasks[white][n] & (position->pieceTables[black][pawn] & ~(1UL << (n+8)))) && !(position->pieceTables[black][pawn] & files[FILE(n)])){
			eval = eval+15;
			//if(wtakes & BIT64(n)){
			//	eval = eval+10;
			//}
		}
	}
#endif*/
	return eval;
}
