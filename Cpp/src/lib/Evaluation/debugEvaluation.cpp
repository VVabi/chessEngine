/*
 * debugEvaluation.cpp
 *
 *  Created on: Oct 8, 2016
 *      Author: vabi
 */


#include <lib/basics.hpp>
#include "evaluation.hpp"
#include <stdlib.h>     /* srand, rand */
#include <lib/Attacks/attacks.hpp>
#include <lib/bitfiddling.h>
#include <communication/gen/VDT.h>
#include <lib/DebugFunctions/debugFunctions.hpp>



extern uint16_t taperingValues[81];

static int32_t debugRookOpenFiles(const chessPosition* position, uint8_t* pawnOccupancy) {
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

int32_t debugEvaluation(const chessPosition* position, VDTevaluation* debugStruct){


	debug_incremental_calculations(position);
	//evaluation from the point of view of WHITE, sign changed in the end if necessary
	//----------------------------------- ---------------------------------------------------
	int32_t eval = 0;
	int32_t bufferMidgame =  calcPieceTableValue(position)/2;
	int32_t bufferEndgame =  calcEndGamePieceTableValue(position);
	uint16_t phase = position->totalFigureEval/100;
#ifdef DEBUG
	if(phase > 80){
		std::cout << "Index out of bounds during tapering eval" << std::endl;
	}
#endif

	int32_t pieceTableEval = ((256-taperingValues[phase])*bufferEndgame+taperingValues[phase]*bufferMidgame)/256; //division by 256
	debugStruct->PSQ = pieceTableEval;

	int32_t figureEval = calcFigureEvaluation(position);
	eval = eval+pieceTableEval+figureEval;
	debugStruct->figureEval = figureEval;



	uint8_t pawnColumnOccupancy[2];

	VDTpawnEvaluation pawnEval;

	int32_t pawnEvalvalue = debugPawnEvaluation(position, pawnColumnOccupancy, phase, &pawnEval);

	eval = eval+pawnEvalvalue;
	pawnEval.total = pawnEvalvalue;
	debugStruct->pawnEval = pawnEval;

	AttackTable whiteAttackTable 		= makeAttackTable(position, white);
	AttackTable blackAttackTable 		= makeAttackTable(position, black);
	int16_t mobility = popcount(whiteAttackTable.completeAttackTable & ~(position->pieces[white]))-popcount(blackAttackTable.completeAttackTable & ~(position->pieces[black]));
	eval = eval+3*mobility;
	debugStruct->mobility = 3*mobility;

	int32_t rookOpenFiles = debugRookOpenFiles(position, pawnColumnOccupancy);

	eval = eval+rookOpenFiles;

	debugStruct->rookOpenFiles = rookOpenFiles;



	uint64_t numWhiteBishops = popcount(position->pieceTables[white][bishop]);

	int32_t bishop_pair = 0;
	if(numWhiteBishops > 1){
		bishop_pair = bishop_pair+50;
	}

	uint64_t numblackBishops = popcount(position->pieceTables[black][bishop]);

	if(numblackBishops > 1){
		bishop_pair = bishop_pair-50;
	}

	eval = eval+bishop_pair;
	debugStruct->bishopPair = bishop_pair;




	/*int32_t kingSafetyComplete = kingSafety(position, pawnColumnOccupancy, &whiteAttackTable, &blackAttackTable);;
	int32_t kingSafetyTapered = (taperingValues[phase]*kingSafetyComplete) >> 8;

	eval = eval+kingSafetyTapered;*/

	//eval = eval+(rand() & 7)-3; //TODO: how is this performance-wise?
	debugStruct->total = eval;
	return (1-2*position->toMove)*eval;

}



