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
#include <DataTypes/vdt_vector.hpp>

#define NUM_DIFFERENT_PIECES 6

#define BIT64(x) (1UL << x)


#define FILE(x) (x & 7)
#define ROW(x)  (x >> 3)

enum figureType: uint16_t {pawn=0, knight=1, bishop=2, rook=3, queen=4, king=5, none};
enum moveType: uint16_t   {pawnMove=0, knightMove=1, bishopMove=2, rookMove=3, queenMove=4, kingMove=5,
	castlingKingside, castlingQueenside, enpassant, promotionQueen, promotionRook, promotionKnight, promotionBishop};
enum playerColor: uint16_t {white=0, black=1};

struct chessMove{
	uint64_t move;
	uint16_t sourceField;
	uint16_t targetField;
	moveType type;
	figureType captureType;
};

struct chessPosition {
#ifdef DEBUG
	vdt_vector<uint64_t> pieces;
	vdt_vector<vdt_vector<uint64_t>> pieceTables;
#else
	uint64_t pieces[2];
	uint64_t pieceTables[2][NUM_DIFFERENT_PIECES+1]; //the +1 is a dummy allowing simpler move execution.
#endif
	playerColor toMove;
	uint8_t castlingRights;
	uint8_t enPassantField; //the field of the CAPTURED pawn
	int16_t figureEval;
	int16_t pieceTableEval;
	uint64_t zobristHash;
	vdt_vector<chessMove> madeMoves;
	vdt_vector<uint16_t> castlingAndEpStack;
};

std::string chessPositionToString(chessPosition position);
std::string chessPositionToOutputString(chessPosition position);
chessPosition stringToChessPosition(std::string strposition);
inline figureType toFigureType(uint16_t num);
std::string moveToString(chessMove move, chessPosition position);
uint64_t stringToMove(std::string mv);
uint64_t getRandUint64();
#endif /* LIB_BASICS_HPP_ */
