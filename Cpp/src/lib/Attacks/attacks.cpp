/*
 * attacks.cpp
 *
 *  Created on: Sep 19, 2016
 *      Author: vabi
 */


#include <lib/basics.hpp>
#include <lib/bitfiddling.h>
#include <lib/Defines/boardParts.hpp>

extern uint64_t knightmovetables[];
extern uint64_t kingmovetables[];

extern uint64_t rookFieldTable[];
extern uint64_t rookMoveTables[64][4096];
extern uint64_t rookMagicNumbers[];

extern uint64_t bishopFieldTable[];
extern uint64_t bishopMoveTables[64][512];
extern uint64_t bishopMagicNumbers[];



bool isFieldAttacked(chessPosition* position, playerColor attackingSide, uint16_t field){

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



