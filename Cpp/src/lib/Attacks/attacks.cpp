/*
 * attacks.cpp
 *
 *  Created on: Sep 19, 2016
 *      Author: vabi
 */


#include <lib/basics.hpp>
#include <lib/bitfiddling.h>
#include <lib/Defines/boardParts.hpp>
#include "attacks.hpp"

extern uint64_t knightmovetables[];
extern uint64_t kingmovetables[];

extern uint64_t rookFieldTable[];
extern uint64_t rookMoveTables[64][4096];
extern uint64_t rookMagicNumbers[];

extern uint64_t bishopFieldTable[];
extern uint64_t bishopMoveTables[64][512];
extern uint64_t bishopMagicNumbers[];



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


AttackTable makeAttackTable(const chessPosition* position, playerColor attackingSide) {

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


