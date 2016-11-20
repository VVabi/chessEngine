/*
 * kingsafety.cpp
 *
 *  Created on: Oct 4, 2016
 *      Author: vabi
 */



#include <lib/basics.hpp>
#include "evaluation.hpp"
#include <lib/bitfiddling.h>
#include <lib/Defines/boardParts.hpp>
#include <lib/Attacks/attacks.hpp>

//extern uint64_t kingmovetables[64];

int32_t attacksCloseToKingEvals[] = {20, 60, 120, 180, 240, 320, 400, 500, 600, 600};
static int32_t kingSafetySinglePlayer(const chessPosition* position, const uint8_t* pawnColumnOccupancy, playerColor playingSide, const AttackTable* opponentAttackTable) {

	int32_t ret = 0;
	//pawn shield
	uint16_t kingField = findLSB(position->pieceTables[playingSide][king]);
	uint16_t kingFile = FILE(kingField);

	if( !((1 << kingFile) & pawnColumnOccupancy[playingSide])) { // no pawn in front of king. TODO: check for pawn really in FRONT of king
		ret = ret-30;
	}

	if( !((1 << kingFile) & pawnColumnOccupancy[1-playingSide])) { // no opponent pawn
		ret = ret-20;
	}

	if(kingFile > 0) {
		if( !((1 << (kingFile-1)) & pawnColumnOccupancy[playingSide])) { // no pawn in front of king. TODO: check for pawn really in FRONT of king
				ret = ret-20;
		}
		if( !((1 << (kingFile-1)) & pawnColumnOccupancy[1-playingSide])) { // no opponent pawn
			ret = ret-10;
		}
	}

	if(kingFile < 7) {
		if( !((1 << (kingFile+1)) & pawnColumnOccupancy[playingSide])) { // no pawn in front of king. TODO: check for pawn really in FRONT of king
				ret = ret-20;
		}
		if( !((1 << (kingFile+1)) & pawnColumnOccupancy[1-playingSide])) { // no opponent pawn
			ret = ret-10;
		}
	}

	/*uint64_t relevant_files = files[FILE(kingField)];
	if(kingFile > 0) {
		relevant_files = relevant_files | files[FILE(kingField-1)];
	}

	if(kingFile < 7) {
		relevant_files = relevant_files | files[FILE(kingField+1)];
	}



	uint64_t opponentPieces = position->pieces[1-playingSide];
	uint64_t opponentQueens = position->pieceTables[1-playingSide][queen];

	uint64_t kingmoves = kingmovetables[kingField];
	kingmoves = kingmoves | BIT64(kingField);

	uint64_t attacks = opponentAttackTable->completeAttackTable & kingmoves;

	ret = ret-attacksCloseToKingEvals[popcount(attacks)];*/

	return (1-2*playingSide)*ret;

}


int32_t kingSafety(const chessPosition* position, const uint8_t* pawnColumnOccupancy, const AttackTable* whiteAttackTable, const AttackTable* blackAttackTable) {

	int32_t whiteSafety =  kingSafetySinglePlayer(position, pawnColumnOccupancy, white, blackAttackTable);
	int32_t blackSafety =  kingSafetySinglePlayer(position, pawnColumnOccupancy, black, whiteAttackTable);
	int32_t ret =  whiteSafety+blackSafety;
	return ret;


}
