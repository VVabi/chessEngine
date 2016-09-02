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

#define BIT64(x) (1UL << x)

enum figureType {pawn=0, knight=1, bishop=2, rook=3, queen=4, king=5, none};
enum moveType   {pawnMove=0, knightMove=1, bishopMove=2, rookMove=3, queenMove=4, kingMove=5,
	castlingSmall, castlingBig, enpassant, promotionQueen, promotionRook, promotionKnight, promotionBishop};
enum playerColor {white=0, black=1};

struct chessMove{
	uint64_t move;
	moveType type;
	figureType captureType;
};

struct chessPosition {
	playerColor toMove;
	uint64_t pieces[2];
	uint64_t pieceTables[2][NUM_DIFFERENT_PIECES+1]; //the +1 is a dummy allowing simpler move execution.
};

std::string chessPositionToString(chessPosition position);
chessPosition stringToChessPosition(std::string strposition);


#endif /* LIB_BASICS_HPP_ */
