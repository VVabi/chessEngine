/*
 * evaluation.cpp
 *
 *  Created on: Sep 25, 2016
 *      Author: vabi
 */




#include <lib/basics.hpp>
#include "evaluation.hpp"
#include <stdlib.h>     /* srand, rand */
#include <lib/Attacks/attacks.hpp>
#include <lib/bitfiddling.h>
bool enablePawnEval = true;


static int32_t rookOpenFiles(chessPosition* position, uint8_t* pawnOccupancy) {
	int32_t ret = 0;

	for(uint8_t color=0; color<2; color++){
		uint64_t rooks = position->pieceTables[color][rook];
		while(rooks) {
			uint16_t field = popLSB(rooks);
			uint16_t file = FILE(field);
			if((pawnOccupancy[color] & (1 << file)) == 0){
				ret = ret+(1-2*color)*30;
			}
		}
	}
	return ret;

}

int32_t evaluation(chessPosition* position, int32_t alpha, int32_t beta){

	//evaluation from the point of view of WHITE, sign changed in the end if necessary
	//--------------------------------------------------------------------------------------
	int32_t eval = 0;
	int32_t buffer =  position->pieceTableEval & 0xFFFF;
	buffer  = buffer-(1 << 15);
	eval = eval+buffer;


	int32_t evalsigned = (1-2*position->toMove)*eval;

	if( (evalsigned < alpha - 500) || (evalsigned > beta+500)) {
		//std::cout << "Futility pruning in eval" << std::endl;
		return evalsigned;
	}


	uint8_t pawnColumnOccupancy[2];
	eval = eval+pawnEvaluation(position, pawnColumnOccupancy);

	AttackTable whiteAttackTable 		= makeAttackTable(position, white);
	AttackTable blackAttackTable 		= makeAttackTable(position, black);
	int16_t mobility = popcount(whiteAttackTable.completeAttackTable & ~(position->pieces[white]))-popcount(blackAttackTable.completeAttackTable & ~(position->pieces[black]));
	eval = eval+3*mobility;


	if(enablePawnEval){
		eval = eval+rookOpenFiles(position, pawnColumnOccupancy);
	}

	eval = eval+(rand() & 7)-3;


	return (1-2*position->toMove)*eval;

}
