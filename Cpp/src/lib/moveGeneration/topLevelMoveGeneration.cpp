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
uint64_t const knightmovetables[] = { KNIGHTMOVES0, KNIGHTMOVES1, KNIGHTMOVES2, KNIGHTMOVES3, KNIGHTMOVES4, KNIGHTMOVES5, KNIGHTMOVES6, KNIGHTMOVES7, KNIGHTMOVES8, KNIGHTMOVES9, KNIGHTMOVES10, KNIGHTMOVES11, KNIGHTMOVES12, KNIGHTMOVES13, KNIGHTMOVES14, KNIGHTMOVES15, KNIGHTMOVES16, KNIGHTMOVES17, KNIGHTMOVES18, KNIGHTMOVES19, KNIGHTMOVES20, KNIGHTMOVES21, KNIGHTMOVES22, KNIGHTMOVES23, KNIGHTMOVES24, KNIGHTMOVES25, KNIGHTMOVES26, KNIGHTMOVES27, KNIGHTMOVES28, KNIGHTMOVES29, KNIGHTMOVES30, KNIGHTMOVES31, KNIGHTMOVES32, KNIGHTMOVES33, KNIGHTMOVES34, KNIGHTMOVES35, KNIGHTMOVES36, KNIGHTMOVES37, KNIGHTMOVES38, KNIGHTMOVES39, KNIGHTMOVES40, KNIGHTMOVES41, KNIGHTMOVES42, KNIGHTMOVES43, KNIGHTMOVES44, KNIGHTMOVES45, KNIGHTMOVES46, KNIGHTMOVES47, KNIGHTMOVES48, KNIGHTMOVES49, KNIGHTMOVES50, KNIGHTMOVES51, KNIGHTMOVES52, KNIGHTMOVES53, KNIGHTMOVES54, KNIGHTMOVES55, KNIGHTMOVES56, KNIGHTMOVES57, KNIGHTMOVES58, KNIGHTMOVES59, KNIGHTMOVES60, KNIGHTMOVES61, KNIGHTMOVES62, KNIGHTMOVES63 };
uint64_t const kingmovetables[] = { KINGMOVES0, KINGMOVES1, KINGMOVES2, KINGMOVES3, KINGMOVES4, KINGMOVES5, KINGMOVES6, KINGMOVES7, KINGMOVES8, KINGMOVES9, KINGMOVES10, KINGMOVES11, KINGMOVES12, KINGMOVES13, KINGMOVES14, KINGMOVES15, KINGMOVES16, KINGMOVES17, KINGMOVES18, KINGMOVES19, KINGMOVES20, KINGMOVES21, KINGMOVES22, KINGMOVES23, KINGMOVES24, KINGMOVES25, KINGMOVES26, KINGMOVES27, KINGMOVES28, KINGMOVES29, KINGMOVES30, KINGMOVES31, KINGMOVES32, KINGMOVES33, KINGMOVES34, KINGMOVES35, KINGMOVES36, KINGMOVES37, KINGMOVES38, KINGMOVES39, KINGMOVES40, KINGMOVES41, KINGMOVES42, KINGMOVES43, KINGMOVES44, KINGMOVES45, KINGMOVES46, KINGMOVES47, KINGMOVES48, KINGMOVES49, KINGMOVES50, KINGMOVES51, KINGMOVES52, KINGMOVES53, KINGMOVES54, KINGMOVES55, KINGMOVES56, KINGMOVES57, KINGMOVES58, KINGMOVES59, KINGMOVES60, KINGMOVES61, KINGMOVES62, KINGMOVES63 };

extern uint64_t rookFieldTable[];
extern uint64_t rookMoveTables[64][4096];
extern uint64_t rookMagicNumbers[];

extern uint64_t bishopFieldTable[];
extern uint64_t bishopMoveTables[64][512];
extern uint64_t bishopMagicNumbers[];

extern uint64_t castlingBlockers[2][2];

inline void extractMoves(uint64_t currentPiece, const figureType figure, uint64_t potentialMoves, vdt_vector<chessMove>* vec, chessPosition* position) {
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
		move.move = nextMove | currentPiece;
		move.type = (moveType) figure;
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
		move.move = target | source;
		move.type = pawnMove;
		move.captureType = none;
		vec->add(&move);
		forward = forward & (~target);
	}

	uint64_t homerow   			= (toMove ? SEVENTHROW : SECONDROW);
	uint64_t onHomeRow 			= pawns & homerow;
	forward   			= (toMove?  onHomeRow >> 16:  onHomeRow << 16);
	uint64_t occupancyShifted 	= (toMove?  occupancy >> 8:  occupancy << 8);
	forward            			= forward & (~occupancy) & (~occupancyShifted);

	while( forward != 0) {
		uint64_t target = LOWESTBITONLY(forward);
		uint64_t source = (toMove? target << 16: target >> 16);
		chessMove move;
		move.move = target | source;
		move.type = pawnMove;
		move.captureType = none;
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
		move.move = target | source;
		move.type = pawnMove;
		move.captureType = captureType;
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
		vec->add(&move);
		takesRight		 	= takesRight & (~target);
	}
}

#define KINGSIDE  0
#define QUEENSIDE 1



void generateCastling(vdt_vector<chessMove>* vec, chessPosition* position){
	playerColor toMove = position->toMove;
	uint8_t castlingMask = (toMove? 12: 3);
	if(position->castlingRights & castlingMask){
		uint64_t occupancy = (position->pieces[white]) | (position->pieces[black]);


		uint8_t castlingOffset = (toMove? 2: 0);

		if ((position->castlingRights & (1 << castlingOffset)) && ((occupancy & castlingBlockers[toMove][KINGSIDE]) == 0)){
			chessMove mv;
			mv.move = (toMove ? BLACKKINGSIDECASTLEMASK: WHITEKINGSIDECASTLEMASK);
			mv.captureType = none;
			mv.type        = castlingKingside;
			vec->add(&mv);
		}

		if ((position->castlingRights & (1 << (castlingOffset+1))) && ((occupancy & castlingBlockers[toMove][QUEENSIDE]) == 0)){
			chessMove mv;
			mv.move = (toMove ? BLACKQUEENSIDECASTLEMASK: WHITEQUEENSIDECASTLEMASK);
			mv.captureType = none;
			mv.type        = castlingQueenside;
			vec->add(&mv);
		}
	}
}


void generateAllMoves(vdt_vector<chessMove>* vec, chessPosition* position) {
	generateCastling(vec, position);
	generateNonSliderMoves(vec, position, knightmovetables, knight);
	generateRookMoves(vec, position, rook);
	generateRookMoves(vec, position, queen);
	generateBishopMoves(vec, position, bishop);
	generateBishopMoves(vec, position, queen);
	generatePawnMoves(vec, position);
	generateNonSliderMoves(vec, position, kingmovetables, king);
}




