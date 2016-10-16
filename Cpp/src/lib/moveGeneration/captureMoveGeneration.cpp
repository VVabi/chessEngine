/*
 * captureMoveGeneration.cpp
 *
 *  Created on: Sep 22, 2016
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

inline void extractCaptureMoves(const uint64_t currentPiece, const figureType figure, uint64_t potentialMoves, vdt_vector<chessMove>* vec, chessPosition* position) {
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
		} else {
			std::cout << "A capture move doesnt actually capture anything - WTF?" << std::endl;
		}
		chessMove move;
		move.move = nextMove | currentPiece;
		move.type = (moveType) figure;
		move.sourceField = sourceField;
		move.targetField = findLSB(nextMove);
		move.captureType = captureType;
		vec->add(&move);
		potentialMoves = potentialMoves & (~nextMove);
	}
}

void generateRookCaptureMoves(vdt_vector<chessMove>* vec, chessPosition* position, const figureType figure) {
	playerColor toMove = position->toMove;
	uint64_t pieces    = position->pieceTables[toMove][figure];
	uint64_t occupancy  = position->pieces[white] | position->pieces[black];
	uint64_t opponentPieces = position->pieces[1-toMove];
	while (pieces != 0) {
		uint64_t nextPiece = LOWESTBITONLY(pieces);
		uint16_t nextPieceField = popLSB(pieces);
		uint64_t magicNumber = rookMagicNumbers[nextPieceField];
		uint64_t blocker = occupancy & rookFieldTable[nextPieceField];
		uint16_t hashValue = (blocker*magicNumber) >> 52;
		uint64_t potentialMoves = rookMoveTables[nextPieceField][hashValue];
		potentialMoves = potentialMoves & opponentPieces;
		extractCaptureMoves(nextPiece, figure, potentialMoves, vec, position);
		pieces = pieces & (~nextPiece);
	}


}

void generateBishopCaptureMoves(vdt_vector<chessMove>* vec, chessPosition* position, const figureType figure) {
	playerColor toMove = position->toMove;
	uint64_t pieces    = position->pieceTables[toMove][figure];
	uint64_t occupancy  = position->pieces[white] | position->pieces[black];
	uint64_t opponentPieces = position->pieces[1-toMove];
	while (pieces != 0) {
		uint64_t nextPiece = LOWESTBITONLY(pieces);
		uint16_t nextPieceField = popLSB(pieces);
		uint64_t magicNumber = bishopMagicNumbers[nextPieceField];
		uint64_t blocker = occupancy & bishopFieldTable[nextPieceField];
		uint16_t hashValue = (blocker*magicNumber) >> 55;
		uint64_t potentialMoves = bishopMoveTables[nextPieceField][hashValue];
		potentialMoves = potentialMoves & opponentPieces;
		extractCaptureMoves(nextPiece, figure, potentialMoves, vec, position);
		pieces = pieces & (~nextPiece);
	}
}

void generateNonSliderCaptureMoves(vdt_vector<chessMove>* vec, chessPosition* position, const uint64_t* moveTable, const figureType figure) {
	playerColor toMove 		= position->toMove;
	uint64_t pieces    		= position->pieceTables[toMove][figure];
	uint64_t opponentPieces = position->pieces[1-toMove];
	while (pieces != 0) {
		uint64_t nextPiece 			= LOWESTBITONLY(pieces);
		uint16_t nextPieceField 	= popLSB(pieces);
		uint64_t potentialMoves 	= moveTable[nextPieceField];
		potentialMoves 				= potentialMoves & opponentPieces;
		extractCaptureMoves(nextPiece, figure, potentialMoves, vec, position);
		pieces 						= pieces & (~nextPiece);
	}
}


void generatePawnCaptureMoves(vdt_vector<chessMove>* vec, chessPosition* position) {
	//TODO: generate promotions! And this function is too complicated
	playerColor toMove = position->toMove;
	uint64_t pawns     = position->pieceTables[toMove][pawn];

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
		move.move = target | source;
		move.type = pawnMove;
		move.captureType = captureType;
		move.sourceField = findLSB(source);
		move.targetField = findLSB(target);
		uint64_t promotionRow = promotionRows[position->toMove];
		if(move.move & promotionRow){
			move.type = promotionQueen;
		}
		vec->add(&move);
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
		move.move = target | source;
		move.type = pawnMove;
		move.captureType = captureType;
		move.sourceField = findLSB(source);
		move.targetField = findLSB(target);
		uint64_t promotionRow = promotionRows[position->toMove];
		if(move.move & promotionRow){
			move.type = promotionQueen;
		}
		vec->add(&move);
		takesRight		 	= takesRight & (~target);
	}
}



static void generateCaptureEnPassant(vdt_vector<chessMove>* vec, chessPosition* position){

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
		mov.move        = BIT64(target) | BIT64(source);
		mov.captureType = pawn;
		mov.type        = enpassant;
		vec->add(&mov);
	}
}


void generateAllCaptureMoves(vdt_vector<chessMove>* vec, chessPosition* position) {
	generateNonSliderCaptureMoves(vec, position, knightmovetables, knight);
	generateNonSliderCaptureMoves(vec, position, kingmovetables, king);
	generateRookCaptureMoves(vec, position, rook);
	generateRookCaptureMoves(vec, position, queen);
	generateBishopCaptureMoves(vec, position, bishop);
	generateBishopCaptureMoves(vec, position, queen);
	generatePawnCaptureMoves(vec, position);
	generateCaptureEnPassant(vec, position);


#ifdef DEBUG


	for(uint16_t ind = 0; ind < vec->length; ind++){
		chessMove mv = (*vec)[ind];
		if( mv.move != (BIT64(mv.sourceField) | BIT64(mv.targetField))) {
			std::cout << "Move source/target is wrong..." << std::endl;

		}

	}


#endif
}


