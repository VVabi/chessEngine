/*
 * moveMaking.cpp
 *
 *  Created on: Sep 2, 2016
 *      Author: vabi
 */

#include <lib/basics.hpp>
#include <lib/Defines/boardParts.hpp>
#include <iostream>
#include <lib/DebugFunctions/debugFunctions.hpp>

extern int16_t figureValues[];
extern int16_t pieceTables[7][2][64];

inline static void makeNormalMove(chessMove* move, chessPosition* position) {
	playerColor toMove 									= position->toMove;
	position->pieces[toMove] 							= position->pieces[toMove]^move->move;
	position->pieces[1-toMove] 							= position->pieces[1-toMove] & (~move->move);
	position->pieceTables[toMove][move->type] 			= position->pieceTables[toMove][move->type]^move->move;
	position->pieceTables[1-toMove][move->captureType] 	= position->pieceTables[1-toMove][move->captureType] & (~move->move);

	position->pieceTableEval = position->pieceTableEval+(1-2*toMove)*(pieceTables[move->type][toMove][move->targetField]-pieceTables[move->type][toMove][move->sourceField]);
	position->pieceTableEval = position->pieceTableEval+(1-2*toMove)*pieceTables[move->captureType][1-toMove][move->targetField];
	position->toMove = (playerColor) (1-position->toMove);
}


//TODO: get rid of the ifs
inline static void makeKingSideCastle(chessMove* move, chessPosition* position) {
	playerColor toMove = position->toMove;

	if(toMove == white){
		position->pieces[toMove] 		     = position->pieces[toMove]^(WHITEKINGSIDECASTLEOCCUPANCYCHANGE);
		position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(WHITEKINGSIDECASTLEROOKMOVE);
		position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(WHITEKINGSIDECASTLEKINGMOVE);
		position->pieceTableEval             = position->pieceTableEval-pieceTables[rook][white][7]+pieceTables[rook][white][5]-pieceTables[king][white][4]+pieceTables[king][white][6];
	} else {
		position->pieces[toMove] 		     = position->pieces[toMove]^(BLACKKINGSIDECASTLEOCCUPANCYCHANGE);
		position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(BLACKKINGSIDECASTLEROOKMOVE);
		position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(BLACKKINGSIDECASTLEKINGMOVE);
		position->pieceTableEval             = position->pieceTableEval-(-pieceTables[rook][black][63]+pieceTables[rook][black][61]-pieceTables[king][black][60]+pieceTables[king][black][62]);
	}
	position->toMove = (playerColor) (1-position->toMove);
}


inline static void makeQueenSideCastle(chessMove* move, chessPosition* position) {
	playerColor toMove = position->toMove;
	if(toMove == white){
		position->pieces[toMove] 		     = position->pieces[toMove]^(WHITEQUEENSIDECASTLEOCCUPANCYCHANGE);
		position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(WHITEQUEENSIDECASTLEROOKMOVE);
		position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(WHITEQUEENSIDECASTLEQUEENMOVE);
		position->pieceTableEval             = position->pieceTableEval-pieceTables[rook][white][0]+pieceTables[rook][white][3]-pieceTables[king][white][4]+pieceTables[king][white][2];
	} else {
		position->pieces[toMove] 		     = position->pieces[toMove]^(BLACKQUEENSIDECASTLEOCCUPANCYCHANGE);
		position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(BLACKQUEENSIDECASTLEROOKMOVE);
		position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(BLACKQUEENSIDECASTLEQUEENMOVE);
		position->pieceTableEval             = position->pieceTableEval-(-pieceTables[rook][black][56]+pieceTables[rook][black][59]-pieceTables[king][black][60]+pieceTables[king][black][58]);
	}
	position->toMove = (playerColor) (1-position->toMove);
}

inline static void makeEnPassant(chessMove* move, chessPosition* position) {
	playerColor toMove 									= position->toMove;
	position->pieces[toMove] 							= position->pieces[toMove]^move->move;
	position->pieceTables[toMove][pawn] 			    = position->pieceTables[toMove][pawn]^move->move;
	uint16_t shift = (toMove? move->targetField+8: move->targetField-8);
	position->pieceTables[1-toMove][move->captureType] 	= position->pieceTables[1-toMove][pawn] ^ BIT64(shift);
	position->pieces[1-toMove] 							= position->pieces[1-toMove] ^BIT64(shift);
	position->pieceTableEval = position->pieceTableEval+(1-2*toMove)*(pieceTables[pawn][toMove][move->targetField]-pieceTables[pawn][toMove][move->sourceField]);
	position->pieceTableEval = position->pieceTableEval+(1-2*toMove)*pieceTables[pawn][1-toMove][shift];
	position->toMove = (playerColor) (1-position->toMove);
}


inline static void makePromotion(chessMove* move, chessPosition* position, figureType promotedFigure) {
	playerColor toMove 									= position->toMove;
	position->pieces[toMove] 							= position->pieces[toMove]^move->move;
	position->pieces[1-toMove] 							= position->pieces[1-toMove] & (~move->move);
	position->pieceTables[toMove][pawn] 				= position->pieceTables[toMove][pawn]  ^ BIT64(move->sourceField);
	position->pieceTables[toMove][promotedFigure] 				= position->pieceTables[toMove][promotedFigure] ^ BIT64(move->targetField);
	position->pieceTables[1-toMove][move->captureType] 	= position->pieceTables[1-toMove][move->captureType] & (~move->move);

	position->pieceTableEval = position->pieceTableEval+(1-2*toMove)*(pieceTables[promotedFigure][toMove][move->targetField]-pieceTables[pawn][toMove][move->sourceField]);
	position->pieceTableEval = position->pieceTableEval+(1-2*toMove)*pieceTables[move->captureType][1-toMove][move->targetField];
	position->figureEval     = position->figureEval+(1-2*toMove)*(figureValues[promotedFigure]-figureValues[pawn]);
	position->toMove = (playerColor) (1-position->toMove);
}

void makeMove(chessMove* move, chessPosition* position) {

	uint16_t to_push = position->castlingRights | (((uint16_t) position->enPassantFile) << 8);
	position->castlingAndEpStack.add(&to_push);
	uint8_t castlingRights = position->castlingRights;
	castlingRights = (move->move & WHITEKINGSIDECASTLEMASK  ? (castlingRights & 14):castlingRights);
	castlingRights = (move->move & WHITEQUEENSIDECASTLEMASK ? (castlingRights & 13):castlingRights);
	castlingRights = (move->move & BLACKKINGSIDECASTLEMASK  ? (castlingRights & 11):castlingRights);
	castlingRights = (move->move & BLACKQUEENSIDECASTLEMASK ? (castlingRights &  7):castlingRights);
	position->castlingRights = castlingRights;
	position->figureEval     = position->figureEval+(1-2*position->toMove)*figureValues[move->captureType];
	position->enPassantFile  = 8;
	switch(move->type){
		case pawnMove:
			if(((move->targetField-move->sourceField) & 15) == 0) { //pawn went two ahead
				position->enPassantFile = FILE(move->targetField);
			}
		case knightMove:
		case bishopMove:
		case rookMove:
		case queenMove:
		case kingMove:
		{
			makeNormalMove(move, position);
			break;
		}
		case castlingKingside:
		{
			makeKingSideCastle(move, position);
			break;
		}
		case castlingQueenside:
		{
			makeQueenSideCastle(move, position);
			break;
		}
		case enpassant:
		{
			makeEnPassant(move, position);
			break;
		}
		case promotionQueen:
		{
			makePromotion(move, position, queen);
			break;
		}
		default:
			break;
	}

	position->madeMoves.add(move);

	#ifdef DEBUG

	int16_t eval = calcFigureEvaluation(position);

	if(eval != position->figureEval){
		std::cout << "Figure evaluation is wrong after moveMake!" << std::endl;
	}

	int16_t pieceTableEval = calcPieceTableValue(position);

	if(pieceTableEval != position->pieceTableEval){
		std::cout << "Piece table evaluation is wrong after moveMake!" << std::endl;
	}

	#endif
}



