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
#include <parameters/parameters.hpp>
#include <logging/logger.hpp>
#include <parameters/parameters.hpp>
#include <lib/Defines/chessFields.hpp>

evaluationResult result;

evaluationResult getEvaluationResult(){
	return result;
}


uint16_t taperingValues[81] = {  0,  0,  0,  0,  0,  0,  0,  0,
						       	 0,  0,  0,  0,  0,  0,  0,  0,
								0, 2, 7, 12, 17, 22, 27, 33,
								39, 45, 51, 57, 64, 71, 78, 85,
								92, 99, 106, 113, 120, 127,134,141,
							   148,155,162,168,174,180,186,192,
							   197,202,206,210,214,218,222,226,
							   229,232,235,238,241,244,247,256,
							   256,256,256,256,256,256,256,256,
							   256,256,256,256,256,256,256,256,256
};

/*
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
*/


#define OUTPOSTVALUE 15

static int16_t outposts(const chessPosition* position) {
	uint64_t wPawns = position->pieceTables[white][pawn];
	uint64_t bPawns = position->pieceTables[black][pawn];
	uint64_t wKnights = position->pieceTables[white][knight];
	uint64_t bKnights = position->pieceTables[black][knight];

	int16_t ret = 0;

	uint64_t wStops = NORTHONE(wPawns);

	uint64_t wPawnAttackSpan = northFill(WESTONE(wStops) | EASTONE(wStops));

	uint64_t bKnightOutposts = bKnights & (~wPawnAttackSpan) & wStops;

	ret = ret-OUTPOSTVALUE*popcount(bKnightOutposts);

	uint64_t bStops = SOUTHONE(bPawns);

	uint64_t bPawnAttackSpan = southFill(WESTONE(bStops) | EASTONE(bStops));

	uint64_t wKnightOutposts = wKnights & (~bPawnAttackSpan) & bStops;

	ret = ret+OUTPOSTVALUE*popcount(wKnightOutposts);
	return ret;

}


static int16_t trappedPieces(const chessPosition* position) {
	uint64_t wPawns = position->pieceTables[white][pawn];
	uint64_t bPawns = position->pieceTables[black][pawn];
	uint64_t wKnights = position->pieceTables[white][knight];
	uint64_t bKnights = position->pieceTables[black][knight];
	uint64_t wBishops = position->pieceTables[white][bishop];
	uint64_t bBishops = position->pieceTables[black][bishop];
	uint64_t wRooks = position->pieceTables[white][rook];
	uint64_t bRooks = position->pieceTables[black][rook];
	int16_t ret = 0;


	if(wBishops & H7) {
		if( (bPawns & G6) && (bPawns & F7)) {
			ret = ret-50;
		}
	}

	if(wBishops & A7) {
		if( (bPawns & B6) && (bPawns & C7)) {
			ret = ret-50;
		}
	}

	if(bBishops & H2) {
		if( (wPawns & G3) && (wPawns & F2)) {
			ret = ret+50;
		}
	}

	if(bBishops & A2) {
		if( (wPawns & B3) && (wPawns & C2)) {
			ret = ret+50;
		}
	}

	if(wKnights & H8) {
		if( (bPawns & (H7 | F7))) {
			ret = ret-50;
		}
	}

	if(wKnights & A8) {
		if( (bPawns & (A7 | C7))) {
			ret = ret-50;
		}
	}

	if(bKnights & H1) {
		if( (wPawns & (H2 | F2))) {
			ret = ret+50;
		}
	}

	if(bKnights & A1) {
		if( (wPawns & (A2 | C2))) {
			ret = ret+50;
		}
	}

	if(bRooks & H8) {
		if((bPawns & (H7 | H6)) && (position->pieceTables[black][king] & G8)) {
			ret = ret+50;
		}
	}

	if(bRooks & A8) {
		if((bPawns & (A7 | A6)) && (position->pieceTables[black][king] & B8)) {
			ret = ret+50;
		}
	}

	if(wRooks & H1) {
		if((wPawns & (H2 | H3)) && (position->pieceTables[white][king] & G1)) {
			ret = ret-50;
		}
	}

	if(wRooks & A1) {
		if((wPawns & (A2 | A3)) && (position->pieceTables[white][king] & B1)) {
			ret = ret-50;
		}
	}

	return ret;

}


static int32_t rookOpenFiles(const chessPosition* position, uint8_t* pawnOccupancy, const evalParameters* evalParams) {
	int32_t ret = 0;

	for(uint8_t color=0; color<2; color++){
		uint64_t rooks = position->pieceTables[color][rook];
		while(rooks) {
			uint16_t field = popLSB(rooks);
			uint16_t file = FILE(field);
/*#ifdef EXPERIMENTAL
			if(((pawnOccupancy[color] | pawnOccupancy[1-color]) & (1 << file)) == 0){
				ret = ret+(1-2*color)*evalParams->rookOnOpenFile;
			}
			else if((pawnOccupancy[color] & (1 << file)) == 0){
				ret = ret+(1-2*color)*5;
			}
#else*/
			if((pawnOccupancy[color] & (1 << file)) == 0){
				ret = ret+(1-2*color)*evalParams->rookOnOpenFile;
			}
//#endif
		}
	}
	return ret;

}

//static uint32_t counter = 0;

int32_t evaluation(const chessPosition* position, int32_t alpha, int32_t beta, bool PSQ_only){


	if( (position->pieceTables[white][pawn] == 0) &&
		(position->pieceTables[black][pawn] == 0) &&
		position->totalFigureEval < 400) {
		return 0; //insufficent material
	}

	const evalParameters* evalPars = getEvalParameters();

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

	if(PSQ_only) {
		return (1-2*position->toMove)*eval;
	}
	result.PSQ = pieceTableEval;

	int32_t evalsigned = (1-2*position->toMove)*eval;

	if( (evalsigned < alpha - 500) || (evalsigned > beta+500)) {
		//std::cout << "Futility pruning in eval" << std::endl;
		return evalsigned;
	}


	int16_t mobilityScore = 0;
	AttackTable whiteAttackTable = makeAttackTableWithMobility(position, white, &mobilityScore);
	eval = eval+mobilityScore;
	result.mobility = mobilityScore;
	mobilityScore = 0;
	AttackTable blackAttackTable = makeAttackTableWithMobility(position, black, &mobilityScore);
	eval = eval-mobilityScore;
	result.mobility = result.mobility-mobilityScore;

	uint8_t pawnColumnOccupancy[2];
	eval = eval+pawnEvaluation(position, pawnColumnOccupancy, phase);

	int16_t rookFiles = rookOpenFiles(position, pawnColumnOccupancy, evalPars);
	eval = eval+rookFiles;
	result.rookOpenFiles = rookFiles;

	uint64_t numWhiteBishops = popcount(position->pieceTables[white][bishop]);

	result.bishoppair = 0;
	if(numWhiteBishops > 1){
		eval 					= eval+evalPars->bishoppair;
		result.bishoppair	 	= evalPars->bishoppair;
	}

	uint64_t numblackBishops = popcount(position->pieceTables[black][bishop]);

	if(numblackBishops > 1){
		eval = eval-evalPars->bishoppair;
		result.bishoppair = result.bishoppair-evalPars->bishoppair;
	}

	int32_t kingSafetyComplete = kingSafety(position, pawnColumnOccupancy, &whiteAttackTable, &blackAttackTable, &evalPars->kingSafetyParameters);
	int32_t kingSafetyTapered = (taperingValues[phase]*kingSafetyComplete)/256;
	eval = eval+kingSafetyTapered;
	result.kingSafety = kingSafetyTapered;

	int16_t trapped = trappedPieces(position);
	eval = eval+trapped;
	result.trappedPieces = trapped;

	int16_t oposts = outposts(position);
	eval = eval+oposts;
	result.outPosts = oposts;

	if(position->toMove == white) {
		eval = eval+10;
	} else {
		eval = eval-10;
	}

/*#ifdef EXPERIMENTAL
	//connected Rooks
	//-----------------
	uint64_t whiteRookAttack = whiteAttackTable.attackTables[rook];
	uint64_t whiteRooks      = position->pieceTables[white][rook];
	if(whiteRookAttack & whiteRooks) {
		eval = eval+10;
	}
	uint64_t blackRookAttack = blackAttackTable.attackTables[rook];
	uint64_t blackRooks      = position->pieceTables[black][rook];
	if(blackRookAttack & blackRooks) {
		eval = eval-10;
	}

#endif*/


#ifdef RANDOMEVAL
	eval = eval+(rand() & 7)-3; //TODO: how is this performance-wise?
#endif
	/*static uint32_t counter = 0;
	counter++;

	if((counter > 100000) && (std::abs(eval-position->figureEval) > 100)){
		counter = 0;
		latexOutput(position, result, eval);
	}*/


	if((position->pieceTables[white][pawn] == 0) && (eval > 0)) {
		if((position->pieceTables[white][rook] | position->pieceTables[white][queen]) == 0) {
			eval = eval/16;
		}
	}


	if((position->pieceTables[black][pawn] == 0) && (eval < 0)) {
		if((position->pieceTables[black][rook] | position->pieceTables[black][queen]) == 0) {
			eval = eval/16;
		}
	}

	return (1-2*position->toMove)*eval;

}
