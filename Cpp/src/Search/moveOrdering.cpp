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
#include <lib/moveMaking/moveMaking.hpp>
#include <parameters/parameters.hpp>



extern int32_t historyTable[2][64][64];
extern int16_t pieceTables[7][2][64];

extern uint16_t killerMoves[20][2];

#define WHITEKINGCASTLECHESSFIELDS ((1UL << 4) | (1UL << 5) | (1UL << 6))
#define WHITEQUEENCASTLECHESSFIELDS ((1UL << 4) | (1UL << 3) | (1UL << 2))
#define BLACKKINGCASTLECHESSFIELDS ((1UL << 60) | (1UL << 61) | (1UL << 62))
#define BLACKQUEENCASTLECHESSFIELDS ((1UL << 60) | (1UL << 59) | (1UL << 58))

uint64_t kingBlockers[] = {WHITEKINGCASTLECHESSFIELDS, BLACKKINGCASTLECHESSFIELDS};
uint64_t queenBlockers[] = {WHITEQUEENCASTLECHESSFIELDS, BLACKQUEENCASTLECHESSFIELDS};

int16_t captureEvals[6][7] = {
		{20, 	130, 	130, 	390, 	900, 	10000,	0},
		{0, 	50, 	50, 	220, 	700, 	10000,	0},
		{0, 	50, 	50, 	220, 	700, 	10000,	0},
		{-20, 	40, 	40, 	210, 	400, 	10000,	0},
		{-70, 	30, 	30, 	160, 	400, 	10000,	0},
		{0, 	200, 	200, 	300, 	700, 	10000,	0},
};

void calcCaptureSortEval(chessPosition* position, chessMove* mv, uint16_t hashedMove) {

	makeMove(mv, position);
	int16_t sortEval = SEE(position, mv);
	undoMove(position);

	/*int16_t sortEval = 0;
	if( ((uint16_t) mv->type) < 6) {
		sortEval = sortEval+captureEvals[mv->type][mv->captureType];
	}*/

	/*if(position->madeMoves.length > 0){
		chessMove previousMove = position->madeMoves[position->madeMoves.length-1];
		if(previousMove.targetField == mv->sourceField) { //recapture is usually good
			sortEval = sortEval + 200;
		}
	}*/


	if( ((mv->sourceField) | (mv->targetField << 8)) == hashedMove) {
		sortEval = sortEval+2000;
	}
	mv->sortEval = sortEval;
}

#define ILLEGAL -20000

static inline void calcSortEval( chessPosition* position, chessMove* mv, AttackTable* opponentAttackTable, AttackTable* ownAttackTable, uint16_t hashedMove, uint16_t killerA, uint16_t killerB, uint16_t refutationTarget) {
	const evalParameters* evalPars 						= getEvalParameters(); //TODO: move outside
	int16_t sortEval = 0;

	if( ((uint16_t) mv->type) < 6) {
		sortEval = sortEval+captureEvals[mv->type][mv->captureType];
	}

	/*if(mv->captureType != none){
		makeMove(mv, position);
		sortEval = SEE(position, mv);
		undoMove(position);
	}*/

	if (((uint16_t) mv->type) < 6) {
		sortEval = sortEval+(pieceTables[mv->type][position->toMove][mv->targetField]-pieceTables[mv->type][position->toMove][mv->sourceField])/4;
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
		sortEval = sortEval-30;
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
		if(!targetCovered){ //TODO: same problem as above, this never happens.
			sortEval = sortEval-100;
		}
	}

	if(targetAttacked && mv->type == kingMove) {
		sortEval = ILLEGAL;
	}

	if( (mv->captureType != none) && ((BIT64(mv->targetField) & opponentAttackTable->completeAttackTable) == 0)) {
		sortEval = sortEval+evalPars->figureValues[mv->captureType];
	}


	if(mv->type == rookMove){

		uint16_t targetFile = FILE(mv->targetField);
		uint64_t pawns      = position->pieceTables[position->toMove][pawn];
		uint64_t opppawns      = position->pieceTables[1-position->toMove][pawn];
		if((targetFile & pawns) == 0) {
			sortEval = sortEval+10;
			if((targetFile & opppawns) == 0){
				sortEval = sortEval+30;
			}
		}

	}


	if((mv->type == pawnMove)) {
		if(BIT64(mv->targetField) & CENTER){
			sortEval = sortEval+80;
		} else if(BIT64(mv->targetField) & WIDECENTER){
			sortEval = sortEval+50;
		} /*else {
			sortEval = sortEval+30;
		}*/
	}
	if((position->madeMoves.length > 0) && (mv->captureType != none)){
		chessMove previousMove = position->madeMoves[position->madeMoves.length-1];
		if(previousMove.targetField == mv->targetField) { //recapture is usually good
			sortEval = sortEval + 200;
		}
	}


	uint16_t moveHash = (mv->sourceField) | (mv->targetField << 8);

	if(moveHash == killerA){
			sortEval = sortEval+120;
	}

	if(moveHash == killerB){
			sortEval = sortEval+119;
	}



	if(mv->type == castlingKingside) {
		sortEval  = 110;
		if(kingBlockers[position->toMove] & opponentAttackTable->completeAttackTable) {
			sortEval = ILLEGAL;
		}
	} else if(mv->type == castlingQueenside) {
		sortEval  = 70;
		if(queenBlockers[position->toMove] & opponentAttackTable->completeAttackTable) {
			sortEval = ILLEGAL;
		}
	}

	if(mv->type == enpassant){
		sortEval = 120;
	}

	if(mv->captureType == none){
		int32_t historyValue = historyTable[position->toMove][mv->sourceField][mv->targetField];
		if(historyValue > 20){
			historyValue = 21+(historyValue/4);
		}

		if(historyValue < -20){
			historyValue = -21+(historyValue/4);
		}

		sortEval = sortEval+historyValue;
		//std::cout << historyValue << std::endl;
		//std::cout << (historyTable[position->toMove][mv->sourceField][mv->targetField] >> 6) << std::endl;
	}

	if( mv->sourceField == refutationTarget) {
		sortEval = sortEval+100;
	}

	if( ((mv->sourceField) | (mv->targetField << 8)) == hashedMove) {
			sortEval = sortEval+2000;
	}

	mv->sortEval = sortEval;
}

bool orderStandardMoves(chessPosition* position, vdt_vector<chessMove>* moves, uint16_t ply, uint16_t hashedMove, uint16_t refutationTarget) {

	AttackTable opponentAttackTable = makeAttackTable(position, (playerColor) (1-position->toMove), position->pieceTables[position->toMove][king]);
	AttackTable ownAttackTable 		= makeAttackTable(position, position->toMove);
	bool isInCheck      = ((opponentAttackTable.completeAttackTable & position->pieceTables[position->toMove][king]) != 0);
	int16_t bestEval = INT16_MIN;
	uint16_t bestIndex = 0;
	uint16_t killerMoveA = killerMoves[ply][0];
	uint16_t killerMoveB = killerMoves[ply][1];
	for (uint16_t ind=0; ind < moves->length; ind++) {
		calcSortEval(position, &(*moves)[ind], &opponentAttackTable, &ownAttackTable, hashedMove, killerMoveA, killerMoveB, refutationTarget);
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

	if(moves->length == 0){
		return;
	}

	uint16_t hashedMove = getHashMove(position->zobristHash);
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
