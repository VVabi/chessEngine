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
			sortEval = sortEval + 20;

		}
	}


	/*if( ((mv->sourceField) | (mv->targetField << 8)) == hashedMove) {
		sortEval = sortEval+2000;
	}*/
	mv->sortEval = sortEval;
}


static inline void calcSortEval(chessPosition* position, chessMove* mv, AttackTable* opponentAttackTable, AttackTable* ownAttackTable, uint16_t hashedMove) {

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
		if(mv->type == kingMove){
			sortEval = sortEval-1000;
		}
		if(!targetCovered){ //TODO: same problem as above, this never happens
			sortEval = sortEval-100;
		}
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

	if( ((mv->sourceField) | (mv->targetField << 8)) == hashedMove) {
		sortEval = sortEval+2000;
	}

	mv->sortEval = sortEval;

}

void orderStandardMoves(chessPosition* position, vdt_vector<chessMove>* moves) {

	AttackTable opponentAttackTable = makeAttackTable(position, (playerColor) (1-position->toMove));
	AttackTable ownAttackTable 		= makeAttackTable(position, position->toMove);
	uint16_t hashedMove = moveOrderingHashTable[position->zobristHash & HASHSIZE];
	for (uint16_t ind=0; ind < moves->length; ind++) {
		calcSortEval(position, &(*moves)[ind], &opponentAttackTable, &ownAttackTable, hashedMove);
	}


	std::sort(moves->data, moves->data+moves->length);


}

void orderCaptureMoves(chessPosition* position, vdt_vector<chessMove>* moves) {

	uint16_t hashedMove = moveOrderingHashTable[position->zobristHash & HASHSIZE] ;
	for (uint16_t ind=0; ind < moves->length; ind++) {
		calcCaptureSortEval(position, &(*moves)[ind], hashedMove);
	}


	std::sort(moves->data, moves->data+moves->length);


}
