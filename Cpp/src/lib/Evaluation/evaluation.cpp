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
#include <userInterface/UIlayer.hpp>
#include <iostream>
#include <fstream>
evaluationResult result;

evaluationResult getEvaluationResult(){
	return result;
}

//std::ofstream out("/home/vabi/TeX/Chess/positions.tex");

uint16_t taperingValues[81] = {  0,  0,  0,  0,  0,  0,  0,  0,
						       	 0,  0,  0,  0,  0,  0,  0,  0,
								0, 2, 7, 11, 14, 19, 24, 29,
								33, 37, 41, 48, 54, 60, 65, 70,
								75, 80,86,92,97,102,108,112,
							   118,124,129,134,140,145,150,155,
							   161,166,173,178,183,189,194,20,
							   206,212,221,230,236,242,248,254,
							   256,256,256,256,256,256,256,256,
							   256,256,256,256,256,256,256,256,256
};

static int32_t rookOpenFiles(const chessPosition* position, uint8_t* pawnOccupancy) {
	int32_t ret = 0;

	for(uint8_t color=0; color<2; color++){
		uint64_t rooks = position->pieceTables[color][rook];
		while(rooks) {
			uint16_t field = popLSB(rooks);
			uint16_t file = FILE(field);
			if((pawnOccupancy[color] & (1 << file)) == 0){
				ret = ret+(1-2*color)*15;
			}
		}
	}
	return ret;

}

extern int16_t endGamepieceTables[7][2][64];

static uint32_t counter = 0;

int32_t evaluation(const chessPosition* position, int32_t alpha, int32_t beta){


	if( (position->pieceTables[white][pawn] == 0) &&
		(position->pieceTables[black][pawn] == 0) &&
		position->totalFigureEval < 400) {
		return 0; //insufficent material
	}

	/*uint16_t blackkingField = findLSB(position->pieceTables[black][king]);
	return endGamepieceTables[king][black][blackkingField];*/ //in nthis way, we actually win endgames vs lone king. But be careful with searchdepth! higher depths may delay the moving of the king
	//into corner indefinitely


	//evaluation from the point of view of WHITE, sign changed in the end if necessary
	//--------------------------------------------------------------------------------------
	int32_t eval = 0;
	int32_t bufferMidgame =  position->pieceTableEval & 0xFFFF;
	bufferMidgame         =  bufferMidgame - (1 << 15);
	int32_t bufferEndgame =  position->pieceTableEval >> 16;
	bufferEndgame  = bufferEndgame-(1 << 14);
	uint16_t phase = position->totalFigureEval/100;

	if(position->pieceTables[white][queen] | position->pieceTables[black][queen]){
		phase = phase+5;
	}


	if(phase > 80){
		phase = 80;
	}


	int32_t pieceTableEval = ((256-taperingValues[phase])*bufferEndgame+taperingValues[phase]*bufferMidgame)/256; //division by 256
	eval = eval+pieceTableEval;
	result.PSQ = pieceTableEval;

	int32_t evalsigned = (1-2*position->toMove)*eval;

	if( (evalsigned < alpha - 500) || (evalsigned > beta+500)) {
		//std::cout << "Futility pruning in eval" << std::endl;
		return evalsigned;
	}


	uint8_t pawnColumnOccupancy[2];
	eval = eval+pawnEvaluation(position, pawnColumnOccupancy, phase);
	int16_t mobilityScore = 0;
	AttackTable whiteAttackTable 		= makeAttackTableWithMobility(position, white, &mobilityScore);
	eval = eval+mobilityScore;
	result.mobility = mobilityScore;
	mobilityScore = 0;
	AttackTable blackAttackTable 		= makeAttackTableWithMobility(position, black, &mobilityScore);
	eval = eval-mobilityScore;
	result.mobility = result.mobility-mobilityScore;


	int16_t rookFiles = rookOpenFiles(position, pawnColumnOccupancy);
	eval = eval+rookFiles;
	result.rookOpenFiles = rookFiles;


	uint64_t numWhiteBishops = popcount(position->pieceTables[white][bishop]);

	result.bishoppair = 0;
	if(numWhiteBishops > 1){
		eval = eval+50;
		result.bishoppair = 50;
	}

	uint64_t numblackBishops = popcount(position->pieceTables[black][bishop]);

	if(numblackBishops > 1){
		eval = eval-50;
		result.bishoppair = result.bishoppair-50;
	}

	int32_t kingSafetyComplete = kingSafety(position, pawnColumnOccupancy, &whiteAttackTable, &blackAttackTable);;
	int32_t kingSafetyTapered = (taperingValues[phase]*kingSafetyComplete)/256;
	eval = eval+kingSafetyTapered;
	result.kingSafety = kingSafetyTapered;
/*#ifdef EXPERIMENTAL
	if(position->toMove == white) {
		eval = eval+10;
	} else {
		eval = eval-10;
	}
#endif*/
#ifdef RANDOMEVAL
	eval = eval+(rand() & 7)-3; //TODO: how is this performance-wise?
#endif
	counter++;

	if((counter > 100000) && (std::abs(eval-position->figureEval) > 100)){
		counter = 0;
		//latexOutput(position, out, result, eval);
	}

	return (1-2*position->toMove)*eval;

}
