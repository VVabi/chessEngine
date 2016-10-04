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


uint16_t taperingValues[81] = {  0,  0,  0,  0,  0,  0,  0,  0,
						       	 0,  0,  0,  0,  0,  0,  0,  5,
								10, 15, 20, 25, 30, 35, 40, 45,
								50, 55, 60, 65, 70, 75, 80, 85,
								90, 95,100,105,110,115,120,125,
							   130,135,140,145,150,155,160,165,
							   170,175,180,185,190,195,200,206,
							   212,218,224,230,236,242,248,254,
							   256,256,256,256,256,256,256,256,
							   256,256,256,256,256,256,256,256,256
};

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
	int32_t bufferMidgame =  position->pieceTableEval & 0xFFFF;
	bufferMidgame         =  bufferMidgame - (1 << 15);
	int32_t bufferEndgame =  position->pieceTableEval >> 16;
	bufferEndgame  = bufferEndgame-(1 << 14);
	uint16_t phase = position->totalFigureEval/100;

#ifdef DEBUG
	if(phase > 80){
		std::cout << "Index out of bounds during tapering eval" << std::endl;
	}
#endif

	int32_t pieceTableEval = ((256-taperingValues[phase])*bufferEndgame+taperingValues[phase]*bufferMidgame) >> 8; //division by 256
	eval = eval+pieceTableEval;


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


	uint64_t numWhiteBishops = popcount(position->pieceTables[white][bishop]);

	if(numWhiteBishops > 1){
		eval = eval+50;
	}

	uint64_t numblackBishops = popcount(position->pieceTables[black][bishop]);

	if(numblackBishops > 1){
		eval = eval-50;
	}




	int32_t kingSafetyComplete = kingSafety(position, pawnColumnOccupancy, &whiteAttackTable, &blackAttackTable);;
	int32_t kingSafetyTapered = (taperingValues[phase]*kingSafetyComplete) >> 8;

	eval = eval+kingSafetyTapered;

	eval = eval+(rand() & 7)-3; //TODO: how is this performance-wise?
	return (1-2*position->toMove)*eval;

}
