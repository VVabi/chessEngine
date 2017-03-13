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
#include <parameters/parameters.hpp>
#include <hashTables/hashTables.hpp>

extern uint64_t files[];
extern uint64_t  passedPawnMasks[2][64];
extern uint16_t taperingValues[81];

static int16_t const passedPawnEvalValues[2][64] = {{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 9, 9, 9, 9, 9, 9, 9, 9, 13, 13, 13, 13, 13, 13, 13, 13,0,0,0,0,0,0,0,0 },
{ 0, 0, 0, 0, 0, 0, 0, 0, 13, 13, 13, 13, 13, 13, 13, 13, 9, 9, 9, 9, 9, 9, 9, 9, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1,0,0,0,0,0,0,0,0 }};


static int16_t kingToPromotionFieldDistance[7][7] = {  // [dist to promotion][king distance from promotion field
		{0,0,0,0,0,0,0 }, //never happens
		{0,0,-10,-20,-30,-45,-60},
		{0,0,-5,-10,-15,-25,-40 },
		{0,0,0,-5,-10,-15,-25 },
		{0,0,0,0,-5,-10,-15 },
		{0,0,0,0,-5,-5,-10 },
		{0,0,0,0,-5,-5,-10 },
};

static uint16_t distBetweenFields(uint16_t a, uint16_t b) {
	uint16_t fileA = FILE(a);
	uint16_t rowA  = ROW(a);
	uint16_t fileB = FILE(b);
	uint16_t rowB  = ROW(b);

	uint16_t rowDiff = (rowA > rowB? rowA-rowB: rowB-rowA);

	uint16_t fileDiff = (fileA > fileB? fileA-fileB : fileB-fileA);

	return std::max(rowDiff, fileDiff);
}

uint64_t staticpawncalls = 0;
uint64_t staticpawnhashhits = 0;

static int32_t passedPawnEval(int32_t* untaperedEval, uint64_t whitePawns, uint64_t blackPawns, uint16_t blackKing, uint16_t whiteKing) {
	//TODO: remove code duplication. Generally this code sucks - too many white/black diffs...
	int32_t eval = 0;
	uint64_t whitePawnBuffer = whitePawns;

	while(whitePawnBuffer){
		uint16_t field = popLSB(whitePawnBuffer);
		if((passedPawnMasks[white][field] & blackPawns) == 0) {

			uint16_t promotionField  = FILE(field)+56;
			uint16_t distToPromotion = 7-ROW(field);
			uint16_t kingDist        = distBetweenFields(promotionField, blackKing);
			if(distToPromotion <= 2) {
				*untaperedEval = *untaperedEval+7*passedPawnEvalValues[white][field];
			} else {
				eval = eval+7*passedPawnEvalValues[white][field];
			}
/*#ifdef EXPERIMENTAL
			int16_t coverage = 0;
			if(distToPromotion < 3) {
				if(whiteAttackTable->attackTables[bishop] & BIT64(promotionField)) {
					coverage = coverage+15;
				}

				uint64_t minorAttacks = blackAttackTable->attackTables[bishop] | blackAttackTable->attackTables[knight];

				if(minorAttacks & BIT64(promotionField)) {
					coverage = coverage-15;
				}

				if(blackAttackTable->attackTables[rook] & BIT64(promotionField)) {
					coverage = coverage-10;
				}

				if(blackAttackTable->attackTables[queen] & BIT64(promotionField)) {
					coverage = coverage-5;
				}
			}
#endif*/

			eval  					 = eval-kingToPromotionFieldDistance[distToPromotion][kingDist];


		}
	}
	uint64_t blackPawnBuffer = blackPawns;
	while(blackPawnBuffer){
		uint16_t field = popLSB(blackPawnBuffer);
		if((passedPawnMasks[black][field] & whitePawns) == 0) {

			uint16_t promotionField  = FILE(field);
			uint16_t distToPromotion = ROW(field);
			uint16_t kingDist        = distBetweenFields(promotionField, whiteKing);

			if(distToPromotion <= 2) {
				*untaperedEval = *untaperedEval-7*passedPawnEvalValues[black][field];
			} else {
				eval = eval-7*passedPawnEvalValues[black][field];
			}

		/*#ifdef EXPERIMENTAL
			int16_t coverage = 0;
			if(distToPromotion < 3) {
				if(blackAttackTable->attackTables[bishop] & BIT64(promotionField)) {
					coverage = coverage-15;
				}

				uint64_t minorAttacks = whiteAttackTable->attackTables[bishop] | whiteAttackTable->attackTables[knight];

				if(minorAttacks & BIT64(promotionField)) {
					coverage = coverage+15;
				}

				if(whiteAttackTable->attackTables[rook] & BIT64(promotionField)) {
					coverage = coverage+10;
				}

				if(whiteAttackTable->attackTables[queen] & BIT64(promotionField)) {
					coverage = coverage+5;
				}

			}
			eval = eval+coverage;
			#endif*/

				eval  					 = eval+kingToPromotionFieldDistance[distToPromotion][kingDist];

		}
	}

	return eval;

}

int32_t staticPawnEval(uint64_t pawns, playerColor color, uint8_t* pawnColumnOccupancy, const staticPawnEvalParameters* staticPawnParameters) { //all stuff depending only on own pawn structure, not the opponents
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

	uint8_t isolatedPawns = ~((*pawnColumnOccupancy << 1) | (*pawnColumnOccupancy >> 1))  & *pawnColumnOccupancy;


	uint64_t isolatedDoublePawns = isolatedPawns & doublePawns;

	uint64_t nonIsolatedDoublePawns =  (~isolatedPawns) & doublePawns;





	eval = eval+staticPawnParameters->isolatedDoublePawn*popcount(isolatedDoublePawns)+staticPawnParameters->nonIsolatedDoublePawn*popcount(nonIsolatedDoublePawns);

	eval = eval+staticPawnParameters->isolatedPawn*popcount(isolatedPawns & (~isolatedDoublePawns));

#ifdef EXPERIMENTAL
	//reward pawns covered by other pawns
	//---------------------------------------
	uint64_t takesRight = (color ? pawns >> 7 : pawns << 9) & NOTFILEA;
	uint64_t takesLeft = (color ? pawns >> 9 : pawns << 7) & NOTFILEH;
	uint64_t takes = takesLeft | takesRight;
	eval = eval+3*popcount(takes & pawns);
#endif

	return eval*(1-2*color);
}

extern evaluationResult result;

int32_t pawnEvaluation(const chessPosition* position, uint8_t* pawnColumnOccupancy, uint16_t phase, const evalParameters* evalPars) {
	uint32_t eval=0;
	uint64_t whitePawns = position->pieceTables[white][pawn];
	uint64_t blackPawns = position->pieceTables[black][pawn];
	int16_t staticPawn = 0;

	pawnHashEntry entry;
	if(getPawnHashTableEntry(&entry, position->pawnHash)) {
		staticPawn = entry.eval;
		staticpawnhashhits++;
		pawnColumnOccupancy[0] = entry.pawnColumnOcc[0];
		pawnColumnOccupancy[1] = entry.pawnColumnOcc[1];
	} else {
		staticpawncalls++;
		staticPawn = staticPawnEval(whitePawns, white, pawnColumnOccupancy,&evalPars->staticPawnParameters)+staticPawnEval(blackPawns, black,  pawnColumnOccupancy+1,&evalPars->staticPawnParameters);
		setPawnHashEntry(staticPawn, pawnColumnOccupancy[0], pawnColumnOccupancy[1], position->pawnHash);
	}
	eval = eval+staticPawn;
	result.staticPawn = staticPawn;
	int32_t untapered = 0;
	int32_t passedPawns = passedPawnEval(&untapered, whitePawns, blackPawns, findLSB(position->pieceTables[black][king]), findLSB(position->pieceTables[white][king]));
/*#ifdef EXPERIMENTAL
	int16_t passedPawnPhase = std::max(phase-10, 0);
#else*/

//#endif
	int16_t passedPawnPhase = std::max((int32_t) phase, 0);
	passedPawns = ((256-taperingValues[passedPawnPhase])*passedPawns)/256;
	eval = eval+passedPawns+untapered;
	result.passedPawn = passedPawns+untapered;


#ifdef NDEF //EXPERIMENTAL  //looks like very small gain (~ 5-6 elo), but I was hoping for more...
	//backwards pawn
	/*int64_t wpawns = position->pieceTables[white][pawn];

	uint64_t wtakesRight =  wpawns << 9 & NOTFILEA;
	uint64_t wtakesLeft =   wpawns << 7 & NOTFILEH;
	uint64_t wtakes     = wtakesLeft | wtakesRight;

	uint64_t bpawns = position->pieceTables[black][pawn];

	uint64_t btakesRight =  bpawns >> 7  & NOTFILEA;
	uint64_t btakesLeft  =  bpawns >> 9  & NOTFILEH;
	uint64_t btakes      =  btakesLeft | btakesRight;*/

	uint64_t buffer = position->pieceTables[white][pawn];
	while(buffer){
		uint16_t n = popLSB(buffer)+8;
		if(!(passedPawnMasks[black][n] & (position->pieceTables[white][pawn] & ~(1ULL << (n-8)))) && !(position->pieceTables[black][pawn] & files[FILE(n)])){
			eval = eval-15;

			/*if(btakes & BIT64(n)){
				eval = eval-10;
			}*/
		}
	}

	buffer = position->pieceTables[black][pawn];
	while(buffer){
		uint16_t n = popLSB(buffer)-8;
		if(!(passedPawnMasks[white][n] & (position->pieceTables[black][pawn] & ~(1ULL << (n+8)))) && !(position->pieceTables[white][pawn] & files[FILE(n)])){
			eval = eval+15;
			/*if(wtakes & BIT64(n)){
				eval = eval+10;
			}*/
		}
	}
#endif
	return eval;
}
