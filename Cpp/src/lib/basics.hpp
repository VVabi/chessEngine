/*
 * basics.hpp
 *
 *  Created on: Sep 1, 2016
 *      Author: vabi
 */

#ifndef LIB_BASICS_HPP_
#define LIB_BASICS_HPP_

#include <stdint.h>
#include <string>

#define NUM_DIFFERENT_PIECES 6

enum figureType {pawn=0, knight=1, bishop=2, rook=3, queen=4, king=5, none};
enum moveType   {pawnMove, knightMove, bishopMove, rookMove, queenMove, kingMove, castling, enpassant};
enum playerColor {white, black};

struct chessMove{
	uint64_t move;
	moveType type;
	figureType captureType;
};

struct chessPosition {
	playerColor toMove;
	uint64_t whitePieces;
	uint64_t whitePieceTables[NUM_DIFFERENT_PIECES];
	uint64_t blackPieces;
	uint64_t blackPieceTables[NUM_DIFFERENT_PIECES];
	uint64_t enpassantFile;
	uint64_t castlingRights;
};

#endif /* LIB_BASICS_HPP_ */
