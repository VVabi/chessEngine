/*
 * attacks.cpp
 *
 *  Created on: Sep 19, 2016
 *      Author: vabi
 */


#include <lib/basics.hpp>
#include <lib/bitfiddling.h>
#include <lib/Defines/boardParts.hpp>
#include <lib/moveMaking/moveMaking.hpp>
#include "attacks.hpp"
#include <assert.h>

extern uint64_t knightmovetables[];
extern uint64_t kingmovetables[];

extern uint64_t rookFieldTable[];
extern uint64_t rookMoveTables[64][4096];
extern uint64_t rookMagicNumbers[];

extern uint64_t bishopFieldTable[];
extern uint64_t bishopMoveTables[64][512];
extern uint64_t bishopMagicNumbers[];
extern int16_t figureValues[];


bool isFieldAttacked(const chessPosition* position, playerColor attackingSide, uint16_t field){

	//knights
	uint64_t knights = position->pieceTables[attackingSide][knight];
	if(knightmovetables[field] & knights) {
		return true;
	}

	//kings
	uint64_t kings = position->pieceTables[attackingSide][king];
	if(kingmovetables[field] & kings) {
		return true;
	}




	uint64_t occupancy = (position->pieces[white]) | (position->pieces[black]);
	//rooks+queens
	uint64_t magicNumber = rookMagicNumbers[field];
	uint64_t blocker = occupancy & rookFieldTable[field];
	uint16_t hashValue = (blocker*magicNumber) >> 52;
	uint64_t potentialMoves = rookMoveTables[field][hashValue];

	if( potentialMoves & (position->pieceTables[attackingSide][rook] | position->pieceTables[attackingSide][queen])) {
		return true;
	}



	//bishops+queens
	magicNumber = bishopMagicNumbers[field];
	blocker = occupancy & bishopFieldTable[field];
	hashValue = (blocker*magicNumber) >> 55;
	potentialMoves = bishopMoveTables[field][hashValue];

	if( potentialMoves & (position->pieceTables[attackingSide][bishop] | position->pieceTables[attackingSide][queen])) {
		return true;
	}

	//pawns
	uint64_t potentialTakes;
	if(attackingSide == white){
		potentialTakes = ((BIT64(field) >> 9) & NOTFILEH)| ((BIT64(field) >> 7) & NOTFILEA);
	} else {
		potentialTakes = ((BIT64(field) << 9) & NOTFILEA) | ((BIT64(field) << 7) & NOTFILEH);
	}

	if(potentialTakes & position->pieceTables[attackingSide][pawn]){
		return true;
	}


	return false;
}


AttackTable makeAttackTable(const chessPosition* position, playerColor attackingSide, uint64_t ignoreMask) {

	AttackTable retTable;
	//pawns
	uint64_t pawns = position->pieceTables[attackingSide][pawn];

	uint64_t takesRight = (attackingSide ? pawns >> 7 : pawns << 9) & NOTFILEA;
	uint64_t takesLeft = (attackingSide ? pawns >> 9 : pawns << 7) & NOTFILEH;
	retTable.attackTables[pawn] = takesLeft | takesRight;

	//knights
	uint64_t knights = position->pieceTables[attackingSide][knight];
	uint64_t knightAttackTable = 0;
	while(knights){
		uint16_t nextKnight = popLSB(knights);
		knightAttackTable = knightAttackTable | knightmovetables[nextKnight];
	}
	retTable.attackTables[knight] = knightAttackTable;

	//bishops
	uint64_t occupancy  = position->pieces[white] | position->pieces[black];
	occupancy = occupancy &(~ignoreMask);
	uint64_t bishops = position->pieceTables[attackingSide][bishop];
	uint64_t bishopAttackTable = 0;
	while (bishops != 0) {
		uint16_t nextPieceField = popLSB(bishops);
		uint64_t magicNumber = bishopMagicNumbers[nextPieceField];
		uint64_t blocker = occupancy & bishopFieldTable[nextPieceField];
		uint16_t hashValue = (blocker*magicNumber) >> 55;
		uint64_t potentialMoves = bishopMoveTables[nextPieceField][hashValue];
		bishopAttackTable = bishopAttackTable | potentialMoves;
	}
	retTable.attackTables[bishop] = bishopAttackTable;
	//rooks

	uint64_t rooks = position->pieceTables[attackingSide][rook];
	uint64_t rookAttackTable = 0;
	while (rooks != 0) {
		uint16_t nextPieceField = popLSB(rooks);
		uint64_t magicNumber = rookMagicNumbers[nextPieceField];
		uint64_t blocker = occupancy & rookFieldTable[nextPieceField];
		uint16_t hashValue = (blocker*magicNumber) >> 52;
		uint64_t potentialMoves = rookMoveTables[nextPieceField][hashValue];
		rookAttackTable = rookAttackTable | potentialMoves;
	}
	retTable.attackTables[rook] = rookAttackTable;

	//queens


	uint64_t queens = position->pieceTables[attackingSide][queen];
	uint64_t queenAttackTable = 0;
	while (queens != 0) {
		uint16_t nextPieceField = popLSB(queens);
		uint64_t magicNumber = rookMagicNumbers[nextPieceField];
		uint64_t blocker = occupancy & rookFieldTable[nextPieceField];
		uint16_t hashValue = (blocker*magicNumber) >> 52;
		uint64_t potentialMoves = rookMoveTables[nextPieceField][hashValue];
		queenAttackTable = queenAttackTable | potentialMoves;
	}

	//TODO: this could be optimized by merging with the rook table and doing both at once
	queens = position->pieceTables[attackingSide][queen];
	while (queens != 0) {
		uint16_t nextPieceField = popLSB(queens);
		uint64_t magicNumber = bishopMagicNumbers[nextPieceField];
		uint64_t blocker = occupancy & bishopFieldTable[nextPieceField];
		uint16_t hashValue = (blocker*magicNumber) >> 55;
		uint64_t potentialMoves = bishopMoveTables[nextPieceField][hashValue];
		queenAttackTable = queenAttackTable | potentialMoves;
	}
	retTable.attackTables[queen] = queenAttackTable;


	//kings
	uint64_t kingField = position->pieceTables[attackingSide][king];
	uint64_t kingAttackTable = 0;
	while(kingField){
		uint16_t nextKing = popLSB(kingField);
		kingAttackTable = kingAttackTable | kingmovetables[nextKing];
	}
	retTable.attackTables[king] = kingAttackTable;
	retTable.completeAttackTable = retTable.attackTables[king] | retTable.attackTables[pawn] | retTable.attackTables[knight] | retTable.attackTables[bishop] | retTable.attackTables[rook] | retTable.attackTables[queen];
	return retTable;
}


int16_t bishopMobility[14]   = {-25,-20,-18,-13,-5,0,3,5,8,10,11,12,12,13};
int16_t rookMobility[15]     = {-25,-20,-18,-15,-10,-8,-3,0,3,5,8,9,10,11,12};
int16_t knightMobility[9]   = {-15,-13,-8,0,3,5,8,9,10};

AttackTable makeAttackTableWithMobility(const chessPosition* position, playerColor attackingSide, int16_t* mobilityScore) {



	*mobilityScore = 0;
	uint64_t ownPieces = position->pieces[attackingSide];
	AttackTable retTable;
	//pawns
	uint64_t pawns = position->pieceTables[attackingSide][pawn];

	uint64_t takesRight = (attackingSide ? pawns >> 7 : pawns << 9) & NOTFILEA;
	uint64_t takesLeft = (attackingSide ? pawns >> 9 : pawns << 7) & NOTFILEH;
	retTable.attackTables[pawn] = takesLeft | takesRight;

	//knights
	uint64_t knights = position->pieceTables[attackingSide][knight];
	uint64_t knightAttackTable = 0;
	while(knights){
		uint16_t nextKnight = popLSB(knights);
		knightAttackTable = knightAttackTable | knightmovetables[nextKnight];
		uint16_t legalMoves = popcount(knightmovetables[nextKnight] & ~ownPieces);
		assert(legalMoves < 9);
		*mobilityScore = *mobilityScore+knightMobility[legalMoves];
	}
	retTable.attackTables[knight] = knightAttackTable;

	//bishops
	uint64_t occupancy  = position->pieces[white] | position->pieces[black];

	uint64_t bishops = position->pieceTables[attackingSide][bishop];
	uint64_t bishopAttackTable = 0;
	while (bishops != 0) {
		uint16_t nextPieceField = popLSB(bishops);
		uint64_t magicNumber = bishopMagicNumbers[nextPieceField];
		uint64_t blocker = occupancy & bishopFieldTable[nextPieceField];
		uint16_t hashValue = (blocker*magicNumber) >> 55;
		uint64_t potentialMoves = bishopMoveTables[nextPieceField][hashValue];
		bishopAttackTable = bishopAttackTable | potentialMoves;
		uint16_t legalMoves = popcount(potentialMoves & ~ownPieces);
		assert(legalMoves < 14);
		*mobilityScore = *mobilityScore+bishopMobility[legalMoves];
	}
	retTable.attackTables[bishop] = bishopAttackTable;
	//rooks

	uint64_t rooks = position->pieceTables[attackingSide][rook];
	uint64_t rookAttackTable = 0;
	while (rooks != 0) {
		uint16_t nextPieceField = popLSB(rooks);
		uint64_t magicNumber = rookMagicNumbers[nextPieceField];
		uint64_t blocker = occupancy & rookFieldTable[nextPieceField];
		uint16_t hashValue = (blocker*magicNumber) >> 52;
		uint64_t potentialMoves = rookMoveTables[nextPieceField][hashValue];
		uint16_t legalMoves = popcount(potentialMoves & ~ownPieces);
		assert(legalMoves < 15);
		*mobilityScore = *mobilityScore+rookMobility[legalMoves];
		rookAttackTable = rookAttackTable | potentialMoves;
	}
	retTable.attackTables[rook] = rookAttackTable;

	//queens


	uint64_t queens = position->pieceTables[attackingSide][queen];
	uint64_t queenAttackTable = 0;
	while (queens != 0) {
		uint16_t nextPieceField = popLSB(queens);
		uint64_t magicNumber = rookMagicNumbers[nextPieceField];
		uint64_t blocker = occupancy & rookFieldTable[nextPieceField];
		uint16_t hashValue = (blocker*magicNumber) >> 52;
		uint64_t potentialMoves = rookMoveTables[nextPieceField][hashValue];
		queenAttackTable = queenAttackTable | potentialMoves;
		*mobilityScore = *mobilityScore+popcount(queenAttackTable & ~ownPieces);
	}

	//TODO: this could be optimized by merging with the rook table and doing both at once
	queens = position->pieceTables[attackingSide][queen];
	while (queens != 0) {
		uint16_t nextPieceField = popLSB(queens);
		uint64_t magicNumber = bishopMagicNumbers[nextPieceField];
		uint64_t blocker = occupancy & bishopFieldTable[nextPieceField];
		uint16_t hashValue = (blocker*magicNumber) >> 55;
		uint64_t potentialMoves = bishopMoveTables[nextPieceField][hashValue];
		queenAttackTable = queenAttackTable | potentialMoves;
	}
	retTable.attackTables[queen] = queenAttackTable;


	//kings
	uint64_t kingField = position->pieceTables[attackingSide][king];
	uint64_t kingAttackTable = 0;
	while(kingField){
		uint16_t nextKing = popLSB(kingField);
		kingAttackTable = kingAttackTable | kingmovetables[nextKing];
	}
	retTable.attackTables[king] = kingAttackTable;
	retTable.completeAttackTable = retTable.attackTables[king] | retTable.attackTables[pawn] | retTable.attackTables[knight] | retTable.attackTables[bishop] | retTable.attackTables[rook] | retTable.attackTables[queen];
	return retTable;
}



bool getNextCapture(chessMove* nextCapture, const chessPosition* position, uint16_t field, figureType occ){

	playerColor attackingSide = position->toMove;
	//pawns
	uint64_t potentialTakes;
	if(attackingSide == white){
		potentialTakes = ((BIT64(field) >> 9) & NOTFILEH)| ((BIT64(field) >> 7) & NOTFILEA);
	} else {
		potentialTakes = ((BIT64(field) << 9) & NOTFILEA) | ((BIT64(field) << 7) & NOTFILEH);
	}

	if(potentialTakes & position->pieceTables[attackingSide][pawn]){
		uint16_t source = findLSB(potentialTakes & position->pieceTables[attackingSide][pawn]);
		nextCapture->captureType = occ;
		nextCapture->move		= BIT64(source) | BIT64(field);
		nextCapture->sourceField = source;
		nextCapture->targetField = field;
		nextCapture->type        = pawnMove;
		return true;
	}


	//knights
	uint64_t knights = position->pieceTables[attackingSide][knight];
	if(knightmovetables[field] & knights) {
		uint16_t source = findLSB(knightmovetables[field] & knights);
		nextCapture->captureType = occ;
		nextCapture->move		= BIT64(source) | BIT64(field);
		nextCapture->sourceField = source;
		nextCapture->targetField = field;
		nextCapture->type        = knightMove;
		return true;
	}
	uint64_t occupancy = (position->pieces[white]) | (position->pieces[black]);

	//bishops
	uint64_t magicNumber = bishopMagicNumbers[field];
	uint64_t blocker = occupancy & bishopFieldTable[field];
	uint16_t hashValue = (blocker*magicNumber) >> 55;
	uint64_t potentialMoves = bishopMoveTables[field][hashValue];

	if(potentialMoves & position->pieceTables[attackingSide][bishop]) {
		uint16_t source = findLSB(potentialMoves & (position->pieceTables[attackingSide][bishop]));
		nextCapture->captureType = occ;
		nextCapture->move		= BIT64(source) | BIT64(field);
		nextCapture->sourceField = source;
		nextCapture->targetField = field;
		nextCapture->type        = bishopMove;
		return true;
	}


	//rooks
	magicNumber = rookMagicNumbers[field];
	blocker = occupancy & rookFieldTable[field];
	hashValue = (blocker*magicNumber) >> 52;
	potentialMoves = rookMoveTables[field][hashValue];

	if( potentialMoves & (position->pieceTables[attackingSide][rook])) {
		uint16_t source = findLSB(potentialMoves & (position->pieceTables[attackingSide][rook]));
		nextCapture->captureType = occ;
		nextCapture->move		 = BIT64(source) | BIT64(field);
		nextCapture->sourceField = source;
		nextCapture->targetField = field;
		nextCapture->type        = rookMove;
		return true;
	}



	//queens
	magicNumber = rookMagicNumbers[field];
	blocker = occupancy & rookFieldTable[field];
	hashValue = (blocker*magicNumber) >> 52;
	potentialMoves = rookMoveTables[field][hashValue];

	if( potentialMoves & (position->pieceTables[attackingSide][queen])) {
		uint16_t source = findLSB(potentialMoves & (position->pieceTables[attackingSide][queen]));
		nextCapture->captureType = occ;
		nextCapture->move		 = BIT64(source) | BIT64(field);
		nextCapture->sourceField = source;
		nextCapture->targetField = field;
		nextCapture->type        = queenMove;
		return true;
	}

	magicNumber = bishopMagicNumbers[field];
	blocker = occupancy & bishopFieldTable[field];
	hashValue = (blocker*magicNumber) >> 55;
	potentialMoves = bishopMoveTables[field][hashValue];

	if(potentialMoves & position->pieceTables[attackingSide][queen]) {
		uint16_t source = findLSB(potentialMoves & (position->pieceTables[attackingSide][queen]));
		nextCapture->captureType = occ;
		nextCapture->move		= BIT64(source) | BIT64(field);
		nextCapture->sourceField = source;
		nextCapture->targetField = field;
		nextCapture->type        = queenMove;
		return true;
	}
	//kings
	uint64_t kings = position->pieceTables[attackingSide][king];
	if(kingmovetables[field] & kings) {
		uint16_t source = findLSB(kingmovetables[field] & kings);
		nextCapture->captureType = occ;
		nextCapture->move		= BIT64(source) | BIT64(field);
		nextCapture->sourceField = source;
		nextCapture->targetField = field;
		nextCapture->type        = kingMove;
		return true;
	}

	return false;

}


int16_t see_internal(int16_t previous, chessPosition* position, uint16_t field, figureType lastCapturingPiece){
	chessMove mv;
	if(!getNextCapture(&mv, position, field, lastCapturingPiece)){
		//no more captures available
		//std::cout << "no further captures found, returning " << -previous << std::endl;
		return -previous; //from POV of player currently moving
	}

	if(mv.captureType == king){ //best-case scenario for us
		return 10000;
	}

	int16_t standPat = -previous;
	makeMove(&mv, position);

	int16_t seeVal = -see_internal(-previous+figureValues[mv.captureType], position, field, (figureType) mv.type);
	undoMove(position);
	/*std::cout << position->madeMoves.length;
	std::cout << "Capturing with " << mv.type << " from " << mv.sourceField << std::endl;
	std::cout << "Standpat score was " << standPat << std::endl;
	std::cout << "see score is " 		<< seeVal << std::endl;*/

	if(seeVal > standPat){
		return seeVal;
	}
	return standPat;
}


int16_t SEE(chessPosition* position, chessMove* mv){

	//TODO: SEE currently cannot handle promotions!!!!
	if(mv->type > 5){
		return 0;
	}
	uint16_t val = figureValues[mv->captureType];
	int16_t ret = -see_internal(val, position, mv->targetField, (figureType) mv->type);
	return ret;
}

