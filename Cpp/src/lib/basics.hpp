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
#include <assert.h>
#define NUM_DIFFERENT_PIECES 6

#define BIT64(x) (1ULL << x)

#define PIECETABLE(a)  			((a & 0xFFFF)-(1 << 15)-64*500)
#define ENDGAMEPIECETABLE(a)  	((a >> 16)-(1 << 15)-64*500)

#define FILE(x) (x & 7)
#define ROW(x)  (x >> 3)

enum figureType: uint16_t {pawn=0, knight=1, bishop=2, rook=3, queen=4, king=5, none=6};
enum moveType: uint16_t   {pawnMove=0, knightMove=1, bishopMove=2, rookMove=3, queenMove=4, kingMove=5,
	castlingKingside=6, castlingQueenside=7, enpassant=8, promotionKnight=9, promotionBishop, promotionRook, promotionQueen};
enum playerColor: uint16_t {white=0, black=1};

struct chessMove{
	uint64_t move;
	uint16_t sourceField;
	uint16_t targetField;
	moveType type;
	figureType captureType;
	int16_t sortEval;
	bool operator<(chessMove move) const{
			return sortEval > move.sortEval;
	}
};

struct pathDependentPositionData{
	uint8_t castlingRights;
	uint8_t enPassantFile; //the file of the CAPTURED pawn
	uint8_t fiftyMoveRuleCounter;
	uint64_t hash;
};

struct chessPosition {
#ifdef DEBUGAAAA
	vdt_vector<uint64_t> pieces;
	vdt_vector<vdt_vector<uint64_t>> pieceTables;
#else
	uint64_t pieces[2];
	uint64_t pieceTables[2][NUM_DIFFERENT_PIECES+1]; //the +1 is a dummy allowing simpler move execution.
#endif
	playerColor toMove;
	pathDependentPositionData data;
	int16_t figureEval;
	uint16_t totalFigureEval;
	uint32_t pieceTableEval;
	uint64_t zobristHash;
	uint64_t pawnHash;
	vdt_vector<chessMove> madeMoves;
	vdt_vector<pathDependentPositionData> dataStack;
};

enum hashFlag: uint16_t {FULLSEARCH=0,FAILLOW=1,FAILHIGH=2};



struct hashEntry{
	uint32_t hashHighBits;
	uint16_t hashLower;
	uint16_t bestMove;
	hashFlag flag;
	uint16_t eval;
	uint8_t depth;
	uint8_t searchId;
	uint16_t index;
} __attribute__((packed));

struct hashBucket{
	hashEntry hashData[4];
};


struct pawnHashEntry{
	uint32_t hashHighBits;
	uint16_t hashLower;
	int16_t  eval;
	uint8_t pawnColumnOcc[2];
};

class timeoutException: public std::exception
{
  virtual const char* what() const throw()
  {
    return "Timeout";
  }
};



uint64_t getRandUint64();
void debug_incremental_calculations(const chessPosition* position);
void free_position(chessPosition* position);
inline figureType toFigureType(uint16_t num);
void zeroInitPosition(chessPosition* position);

#endif /* LIB_BASICS_HPP_ */
