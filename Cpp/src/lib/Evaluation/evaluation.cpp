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

static int32_t rookOpenFiles(const chessPosition* position, uint8_t* pawnOccupancy) {
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

extern int16_t endGamepieceTables[7][2][64];

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

//#ifdef DEBUG
	if(phase > 80){
		//std::cout << "Index out of bounds during tapering eval" << std::endl
		phase = 80;
	}
//#endif

	int32_t pieceTableEval = ((256-taperingValues[phase])*bufferEndgame+taperingValues[phase]*bufferMidgame)/256; //division by 256
	eval = eval+pieceTableEval;


	int32_t evalsigned = (1-2*position->toMove)*eval;

	if( (evalsigned < alpha - 500) || (evalsigned > beta+500)) {
		//std::cout << "Futility pruning in eval" << std::endl;
		return evalsigned;
	}


	uint8_t pawnColumnOccupancy[2];
	eval = eval+pawnEvaluation(position, pawnColumnOccupancy, phase);
#ifdef EXPERIMENTAL
	int16_t mobilityScore = 0;
	AttackTable whiteAttackTable 		= makeAttackTableWithMobility(position, white, &mobilityScore);
	eval = eval+mobilityScore/2;
	mobilityScore = 0;
	AttackTable blackAttackTable 		= makeAttackTableWithMobility(position, black, &mobilityScore);
	eval = eval-mobilityScore/2;
#else
	int16_t mobilityScore = 0;
	AttackTable whiteAttackTable 		= makeAttackTableWithMobility(position, white, &mobilityScore);
	eval = eval+mobilityScore;
	mobilityScore = 0;
	AttackTable blackAttackTable 		= makeAttackTableWithMobility(position, black, &mobilityScore);
	eval = eval-mobilityScore;
#endif


	eval = eval+rookOpenFiles(position, pawnColumnOccupancy);


	uint64_t numWhiteBishops = popcount(position->pieceTables[white][bishop]);

	if(numWhiteBishops > 1){
		eval = eval+50;
	}

	uint64_t numblackBishops = popcount(position->pieceTables[black][bishop]);

	if(numblackBishops > 1){
		eval = eval-50;
	}

//#ifdef EXPERIMENTAL
	int32_t kingSafetyComplete = kingSafety(position, pawnColumnOccupancy, &whiteAttackTable, &blackAttackTable);;
	int32_t kingSafetyTapered = (taperingValues[phase]*kingSafetyComplete)/256;
	eval = eval+kingSafetyTapered;
//#endif

#ifdef RANDOMEVAL
	eval = eval+(rand() & 7)-3; //TODO: how is this performance-wise?
#endif
	return (1-2*position->toMove)*eval;

}
