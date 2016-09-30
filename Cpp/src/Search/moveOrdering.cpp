/*
 * moveOrdering.cpp
 *
 *  Created on: Sep 22, 2016
 *      Author: vabi
 */

#include <DataTypes/vdt_vector.hpp>
#include <lib/basics.hpp>
#include <lib/Defines/knightmoves.hpp>
#include <lib/Defines/kingMoves.hpp>
#include <lib/Defines/boardParts.hpp>
#include <lib/bitfiddling.h>
#include <algorithm>
#include <lib/Attacks/attacks.hpp>
#include <hashTables/hashTables.hpp>


extern uint16_t moveOrderingHashTable[];



extern int16_t figureValues[];
extern int16_t pieceTables[7][2][64];

extern uint16_t killerMoves[20][2];

#define WHITEKINGCASTLECHESSFIELDS ((1UL << 4) | (1UL << 5) | (1UL << 6))
#define WHITEQUEENCASTLECHESSFIELDS ((1UL << 4) | (1UL << 3) | (1UL << 2))
#define BLACKKINGCASTLECHESSFIELDS ((1UL << 60) | (1UL << 61) | (1UL << 62))
#define BLACKQUEENCASTLECHESSFIELDS ((1UL << 60) | (1UL << 59) | (1UL << 58))

uint64_t kingBlockers[] = {WHITEKINGCASTLECHESSFIELDS, BLACKKINGCASTLECHESSFIELDS};
uint64_t queenBlockers[] = {WHITEQUEENCASTLECHESSFIELDS, BLACKQUEENCASTLECHESSFIELDS};

int16_t captureEvals[6][7] = {
		{20, 250, 260, 490, 900, 10000,0},
		{0, 20, 20, 290, 700, 10000,0},
		{0, 20, 20, 290, 700, 10000,0},
		{-20, 0, 0, 40, 400, 10000,0},
		{-70, -20, -20, 40, 100, 10000,0},
		{100, 200, 200, 300, 400, 10000,0},
};

void calcCaptureSortEval(chessPosition* position, chessMove* mv, uint16_t hashedMove) {

	int16_t sortEval = 0;
	if( ((uint16_t) mv->type) < 6) {
		sortEval = sortEval+captureEvals[mv->type][mv->captureType];
	}

	if(position->madeMoves.length > 0){
		chessMove previousMove = position->madeMoves[position->madeMoves.length];
		if(previousMove.targetField == mv->sourceField) { //recapture is usually good
			sortEval = sortEval + 200;
		}
	}


	if( ((mv->sourceField) | (mv->targetField << 8)) == hashedMove) {
		sortEval = sortEval+2000;
	}
	mv->sortEval = sortEval;
}


static inline void calcSortEval(chessPosition* position, chessMove* mv, AttackTable* opponentAttackTable, AttackTable* ownAttackTable, uint16_t hashedMove, uint16_t killerA, uint16_t killerB) {

	int16_t sortEval = 0;

	if( ((uint16_t) mv->type) < 6) {
		sortEval = sortEval+captureEvals[mv->type][mv->captureType];
	}

	if (((uint16_t) mv->type) < 6) {
		sortEval = sortEval+pieceTables[mv->type][position->toMove][mv->targetField]-pieceTables[mv->type][position->toMove][mv->sourceField];
	}

	if(mv->type == promotionQueen) {
		sortEval = sortEval+300;
	}


	bool sourceAttacked = false;
	bool targetAttacked = false;
	bool sourceCovered  = false;
	bool targetCovered  = false; //TODO: this doesnt work as intended - the target of a move is usually covered... - or may be covered after the move, eg. h7h5 from startposition, even if it wasnt before



	if(BIT64(mv->sourceField) & ownAttackTable->completeAttackTable) {
		sourceCovered = true;
	}
	if(BIT64(mv->targetField) & ownAttackTable->completeAttackTable) {
		targetCovered= true; //hm that can ONLY be false for some pawn moves actually...
	}

	if(BIT64(mv->sourceField) & opponentAttackTable->completeAttackTable) {
		sourceAttacked = true;
	}
	if(BIT64(mv->targetField) & opponentAttackTable->completeAttackTable) {
		targetAttacked = true;
	}

	if( (mv->captureType == none) && (mv->type > pawnMove) && (BIT64(mv->targetField) & opponentAttackTable->attackTables[pawn])) {
		sortEval = sortEval-100;
	}


	if(sourceCovered){
		sortEval = sortEval-20;
	}

	if(targetCovered) {
		sortEval = sortEval+20;
	}

	if(sourceAttacked){
		sortEval = sortEval+100;
		if(!sourceCovered) {
			sortEval = sortEval+100;
		}
		if(mv->type == kingMove){
			sortEval = sortEval+400;
		}
	}

	if(targetAttacked  && (mv->captureType == none)){
		sortEval = sortEval-100;
		if(!targetCovered){ //TODO: same problem as above, this never happens
			sortEval = sortEval-100;
		}
	}

	if(targetAttacked && mv->type == kingMove) {
		sortEval = INT16_MIN;
	}

	if( (mv->captureType != none) && ((BIT64(mv->targetField) & opponentAttackTable->completeAttackTable) == 0)) {
		sortEval = sortEval+figureValues[mv->captureType];
	}

	if((position->madeMoves.length > 0) && (mv->captureType != none)){
		chessMove previousMove = position->madeMoves[position->madeMoves.length];
		if(previousMove.targetField == mv->sourceField) { //recapture is usually good
			sortEval = sortEval + 200;
		}
	}


	uint16_t moveHash = (mv->sourceField) | (mv->targetField << 8);

	if(moveHash == killerA){
			sortEval = sortEval+90;
	}

	if(moveHash == killerB){
			sortEval = sortEval+89;
	}

	if( ((mv->sourceField) | (mv->targetField << 8)) == hashedMove) {
		sortEval = sortEval+2000;
	}

	if(mv->type == castlingKingside) {
		sortEval  = 80;

		if(kingBlockers[position->toMove] & opponentAttackTable->completeAttackTable) {
			sortEval = INT16_MIN;
		}


	} else if(mv->type == castlingQueenside) {
		sortEval  = 40;
		if(queenBlockers[position->toMove] & opponentAttackTable->completeAttackTable) {
			sortEval = INT16_MIN;
		}
	}

	if(mv->type == enpassant){
		sortEval = 120;
	}
	mv->sortEval = sortEval;
}

bool orderStandardMoves(chessPosition* position, vdt_vector<chessMove>* moves, uint16_t depth) {

	AttackTable opponentAttackTable = makeAttackTable(position, (playerColor) (1-position->toMove));
	AttackTable ownAttackTable 		= makeAttackTable(position, position->toMove);
	uint16_t hashedMove = moveOrderingHashTable[position->zobristHash & HASHSIZE];
	bool isInCheck      = ((opponentAttackTable.completeAttackTable & position->pieceTables[position->toMove][king]) != 0);
	int16_t bestEval = INT16_MIN;
	uint16_t bestIndex = 0;
	uint16_t killerMoveA = killerMoves[depth][0];
	uint16_t killerMoveB = killerMoves[depth][1];
	for (uint16_t ind=0; ind < moves->length; ind++) {
		calcSortEval(position, &(*moves)[ind], &opponentAttackTable, &ownAttackTable, hashedMove, killerMoveA, killerMoveB);
		if((*moves)[ind].sortEval > bestEval){
			bestEval = (*moves)[ind].sortEval;
			bestIndex = ind;
		}
	}

	chessMove buffer = (*moves)[0];
	(*moves)[0] = (*moves)[bestIndex];
	(*moves)[bestIndex] = buffer;


	//std::sort(moves->data, moves->data+moves->length);
	return isInCheck;

}

void orderCaptureMoves(chessPosition* position, vdt_vector<chessMove>* moves) {

	uint16_t hashedMove = moveOrderingHashTable[position->zobristHash & HASHSIZE] ;
	int16_t bestEval = INT16_MIN;
	uint16_t bestIndex = 0;

	for (uint16_t ind=0; ind < moves->length; ind++) {
		calcCaptureSortEval(position, &(*moves)[ind], hashedMove);
		if((*moves)[ind].sortEval > bestEval){
			bestEval = (*moves)[ind].sortEval;
			bestIndex = ind;
		}
	}

	chessMove buffer = (*moves)[0];
	(*moves)[0] = (*moves)[bestIndex];
	(*moves)[bestIndex] = buffer;

	//std::sort(moves->data, moves->data+moves->length);


}
