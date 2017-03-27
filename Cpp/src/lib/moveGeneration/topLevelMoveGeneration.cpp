/*
 * topLevelMoveGeneration.cpp
 *
 *  Created on: Sep 1, 2016
 *      Author: vabi
 */


#include <DataTypes/vdt_vector.hpp>
#include <lib/basics.hpp>
#include <lib/Defines/knightmoves.hpp>
#include <lib/Defines/kingMoves.hpp>
#include <lib/Defines/boardParts.hpp>
#include <lib/bitfiddling.h>


//TODO: for debug purposes I want these rangechecked!
extern uint64_t knightmovetables[];
extern uint64_t kingmovetables[];
extern uint64_t rookFieldTable[];
extern uint64_t rookMoveTables[64][4096];
extern uint64_t rookMagicNumbers[];

extern uint64_t bishopFieldTable[];
extern uint64_t bishopMoveTables[64][512];
extern uint64_t bishopMagicNumbers[];

extern uint64_t castlingBlockers[2][2];
extern uint64_t enPassantMoveTable[2][8];
extern uint16_t enPassantTargetFields[2][8];

static uint64_t promotionRows[] = {LASTROW, FIRSTROW};

inline void extractMoves(const uint64_t currentPiece, const figureType figure, uint64_t potentialMoves, vdt_vector<chessMove>* vec, chessPosition* position) {
	uint16_t sourceField = findLSB(currentPiece);
	playerColor toMove = position->toMove;
	while (potentialMoves != 0) {
		uint64_t nextMove = LOWESTBITONLY(potentialMoves);
		figureType captureType = none;
		if (nextMove & position->pieces[1-toMove]) {
			for(uint16_t ind=0; ind < NUM_DIFFERENT_PIECES; ind++) {
				if(nextMove & position->pieceTables[1-toMove][ind]) {
					captureType = (figureType) ind;
					break;
				}
			}
		}
		chessMove move;
		move.type = (moveType) figure;
		move.sourceField = sourceField;
		move.targetField = findLSB(nextMove);
		move.captureType = captureType;
		vec->add(&move);
		potentialMoves = potentialMoves & (~nextMove);
	}
}

void generateRookMoves(vdt_vector<chessMove>* vec, chessPosition* position, const figureType figure) {
	playerColor toMove = position->toMove;
	uint64_t pieces    = position->pieceTables[toMove][figure];
	uint64_t occupancy  = position->pieces[white] | position->pieces[black];
	while (pieces != 0) {
		uint64_t nextPiece = LOWESTBITONLY(pieces);
		uint16_t nextPieceField = popLSB(pieces);
		uint64_t magicNumber = rookMagicNumbers[nextPieceField];
		uint64_t blocker = occupancy & rookFieldTable[nextPieceField];
		uint16_t hashValue = (blocker*magicNumber) >> 52;
		uint64_t potentialMoves = rookMoveTables[nextPieceField][hashValue];
		potentialMoves = potentialMoves & (~position->pieces[toMove]);
		extractMoves(nextPiece, figure, potentialMoves, vec, position);
		pieces = pieces & (~nextPiece);
	}


}

void generateBishopMoves(vdt_vector<chessMove>* vec, chessPosition* position, const figureType figure) {
	playerColor toMove = position->toMove;
	uint64_t pieces    = position->pieceTables[toMove][figure];
	uint64_t occupancy  = position->pieces[white] | position->pieces[black];
	while (pieces != 0) {
		uint64_t nextPiece = LOWESTBITONLY(pieces);
		uint16_t nextPieceField = popLSB(pieces);
		uint64_t magicNumber = bishopMagicNumbers[nextPieceField];
		uint64_t blocker = occupancy & bishopFieldTable[nextPieceField];
		uint16_t hashValue = (blocker*magicNumber) >> 55;
		uint64_t potentialMoves = bishopMoveTables[nextPieceField][hashValue];
		potentialMoves = potentialMoves & (~position->pieces[toMove]);
		extractMoves(nextPiece, figure, potentialMoves, vec, position);
		pieces = pieces & (~nextPiece);
	}
}

void generateNonSliderMoves(vdt_vector<chessMove>* vec, chessPosition* position, const uint64_t* moveTable, const figureType figure) {
	playerColor toMove = position->toMove;
	uint64_t pieces    = position->pieceTables[toMove][figure];
	while (pieces != 0) {
		uint64_t nextPiece = LOWESTBITONLY(pieces);
		uint16_t nextPieceField = popLSB(pieces);
		uint64_t potentialMoves = moveTable[nextPieceField];
		potentialMoves = potentialMoves & (~position->pieces[toMove]);
		extractMoves(nextPiece, figure, potentialMoves, vec, position);
		pieces = pieces & (~nextPiece);
	}
}


void generatePawnMoves(vdt_vector<chessMove>* vec, chessPosition* position) {
	//TODO: generate promotions! And this function is too complicated
	playerColor toMove = position->toMove;
	uint64_t occupancy  = position->pieces[white] | position->pieces[black];
	uint64_t pawns     = position->pieceTables[toMove][pawn];
	uint64_t forward   = (toMove? pawns >> 8: pawns << 8);
	forward            = forward & (~occupancy);

	while (forward != 0)  {
		uint64_t target = LOWESTBITONLY(forward);
		uint64_t source = (toMove? target << 8: target >> 8);
		chessMove move;
		move.type = pawnMove;
		move.captureType = none;
		move.sourceField = findLSB(source);
		move.targetField = findLSB(target);
		uint64_t promotionRow = promotionRows[position->toMove];
		if(target & promotionRow){
			move.type = promotionQueen;
			vec->add(&move);
			move.type = promotionRook;
			vec->add(&move);
			move.type = promotionBishop;
			vec->add(&move);
			move.type = promotionKnight;
			vec->add(&move);
		} else {
			vec->add(&move);
		}
		forward = forward & (~target);
	}

	uint64_t homerow   			= (toMove ? SEVENTHROW : SECONDROW);
	uint64_t onHomeRow 			= pawns & homerow;
	forward   					= (toMove?  onHomeRow >> 16:  onHomeRow << 16);
	uint64_t occupancyShifted 	= (toMove?  occupancy >> 8:  occupancy << 8);
	forward            			= forward & (~occupancy) & (~occupancyShifted);

	while( forward != 0) {
		uint64_t target = LOWESTBITONLY(forward);
		uint64_t source = (toMove? target << 16: target >> 16);
		chessMove move;
		move.type = pawnMove;
		move.captureType = none;
		move.sourceField = findLSB(source);
		move.targetField = findLSB(target);
		vec->add(&move);
		forward = forward & (~target);
	}

	uint64_t takesLeft = (toMove? pawns >> 9 : pawns << 7) & NOTFILEH & position->pieces[1-toMove];

	while(takesLeft){
		uint64_t target 	= LOWESTBITONLY(takesLeft);
		uint64_t source 	= (toMove? target << 9: target >> 7);
		figureType captureType = none;
		for(uint16_t ind=0; ind < NUM_DIFFERENT_PIECES; ind++) {
			if(target & position->pieceTables[1-toMove][ind]) {
				captureType = (figureType) ind;
				break;
			}
		}
		#ifdef DEBUG
		if(captureType == none){
			std::cout << "Pawn capture move generation is buggy" << std::endl;

		}
		#endif
		chessMove move;
		move.type = pawnMove;
		move.captureType = captureType;
		move.sourceField = findLSB(source);
		move.targetField = findLSB(target);
		uint64_t promotionRow = promotionRows[position->toMove];
		if(target & promotionRow){
			move.type = promotionQueen;
			vec->add(&move);
			move.type = promotionRook;
			vec->add(&move);
			move.type = promotionBishop;
			vec->add(&move);
			move.type = promotionKnight;
			vec->add(&move);
		} else {
			vec->add(&move);
		}
		takesLeft		 	= takesLeft & (~target);
	}

	uint64_t takesRight = (toMove? pawns >> 7 : pawns << 9) & NOTFILEA & position->pieces[1-toMove];

	while(takesRight){
		uint64_t target 	= LOWESTBITONLY(takesRight);
		uint64_t source 	= (toMove? target << 7: target >> 9);
		figureType captureType = none;
		for(uint16_t ind=0; ind < NUM_DIFFERENT_PIECES; ind++) {
			if(target & position->pieceTables[1-toMove][ind]) {
				captureType = (figureType) ind;
				break;
			}
		}
		#ifdef DEBUG
		if(captureType == none){
			std::cout << "Pawn capture move generation is buggy" << std::endl;
		}
		#endif
		chessMove move;
		move.type = pawnMove;
		move.captureType = captureType;
		move.sourceField = findLSB(source);
		move.targetField = findLSB(target);
		uint64_t promotionRow = promotionRows[position->toMove];
		if(target & promotionRow){
			move.type = promotionQueen;
			vec->add(&move);
			move.type = promotionRook;
			vec->add(&move);
			move.type = promotionBishop;
			vec->add(&move);
			move.type = promotionKnight;
			vec->add(&move);
		} else {
			vec->add(&move);
		}
		takesRight		 	= takesRight & (~target);
	}
}

#define KINGSIDE  0
#define QUEENSIDE 1



void generateCastling(vdt_vector<chessMove>* vec, chessPosition* position){
	playerColor toMove = position->toMove;
	uint8_t castlingMask = (toMove? 12: 3);
	if(position->data.castlingRights & castlingMask){
		uint64_t occupancy = (position->pieces[white]) | (position->pieces[black]);


		uint8_t castlingOffset = (toMove? 2: 0);

		if ((position->data.castlingRights & (1 << castlingOffset)) && ((occupancy & castlingBlockers[toMove][KINGSIDE]) == 0)){
			chessMove mv;
			mv.captureType = none;
			mv.type        = castlingKingside;
			mv.sourceField = (toMove ? 60: 4);
			mv.targetField = (toMove ? 63: 7);
			vec->add(&mv);
		}

		if ((position->data.castlingRights & (1 << (castlingOffset+1))) && ((occupancy & castlingBlockers[toMove][QUEENSIDE]) == 0)){
			chessMove mv;
			mv.captureType = none;
			mv.type        = castlingQueenside;
			mv.sourceField = (toMove ? 60: 4);
			mv.targetField = (toMove ? 56: 0);
			vec->add(&mv);
		}
	}
}

static void generateEnPassant(vdt_vector<chessMove>* vec, chessPosition* position){

	if(position->data.enPassantFile > 7){
		return;
	}

	uint64_t mask = (enPassantMoveTable[position->toMove][position->data.enPassantFile]) & (position->pieceTables[position->toMove][pawn]);

	while(mask) {
		uint16_t source = popLSB(mask);
		uint16_t target = enPassantTargetFields[position->toMove][position->data.enPassantFile];

		chessMove mov;
		mov.sourceField = source;
		mov.targetField = target;
		mov.captureType = pawn;
		mov.type        = enpassant;
		vec->add(&mov);
	}
}


void generateAllMoves(vdt_vector<chessMove>* vec, chessPosition* position) {
	generateCastling(vec, position);
	generateNonSliderMoves(vec, position, knightmovetables, knight);
	generateNonSliderMoves(vec, position, kingmovetables, king);
	generateRookMoves(vec, position, rook);
	generateRookMoves(vec, position, queen);
	generateBishopMoves(vec, position, bishop);
	generateBishopMoves(vec, position, queen);
	generatePawnMoves(vec, position);
	generateEnPassant(vec, position);

}




