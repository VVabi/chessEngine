/*
 * basicTypes.hpp
 *
 *  Created on: Aug 27, 2017
 *      Author: vabi
 */

#ifndef LIB_BASICTYPES_HPP_
#define LIB_BASICTYPES_HPP_

#define NUM_DIFFERENT_PIECES 6

#define BIT64(x) (1ULL << (x))

#define PIECETABLE(a)           (((a) & 0xFFFF)-(1 << 15)-64*500)
#define ENDGAMEPIECETABLE(a)    (((a) >> 16)-(1 << 15)-64*500)

#define FILE(x) ((x) & 7)
#define ROW(x)  ((x) >> 3)


enum figureType: uint8_t {pawn = 0, knight = 1, bishop = 2, rook = 3, queen = 4, king = 5, none = 6};
enum moveType: uint8_t   {pawnMove = 0, knightMove = 1, bishopMove = 2, rookMove = 3, queenMove = 4, kingMove = 5,
    castlingKingside = 6, castlingQueenside = 7, enpassant = 8, promotionKnight = 9, promotionBishop, promotionRook, promotionQueen};
enum playerColor: uint16_t {white = 0, black = 1};

#define INVERTCOLOR(c) ((playerColor) (1-(c)))
#define COLORSIGN(c)  (1-2*c)


#endif /* LIB_BASICTYPES_HPP_ */
