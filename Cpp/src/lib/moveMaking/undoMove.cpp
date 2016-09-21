/*
 * undoMove.cpp
 *
 *  Created on: Sep 21, 2016
 *      Author: vabi
 */

#include <lib/basics.hpp>
#include <lib/Defines/boardParts.hpp>
#include <iostream>
#include <lib/DebugFunctions/debugFunctions.hpp>

extern int16_t figureValues[];
extern int16_t pieceTables[7][2][64];


inline static void undoNormalMove(chessPosition* position, chessMove move) {
	position->toMove = (playerColor) (1-position->toMove);
	playerColor toMove 									= position->toMove;
	position->pieces[toMove] 							= position->pieces[toMove]^move.move;
	position->pieceTables[toMove][move.type] 			= position->pieceTables[toMove][move.type]^move.move;
	uint64_t isCapture = ((none == move.captureType) ? 0:UINT64_MAX);
	position->pieces[1-toMove] 							= (position->pieces[1-toMove] | (isCapture & move.move)) & (~position->pieces[toMove]);
	position->pieceTables[1-toMove][move.captureType] 	= (position->pieceTables[1-toMove][move.captureType] | (isCapture & move.move)) & (~position->pieces[toMove]);
	position->pieceTableEval = position->pieceTableEval-(1-2*toMove)*(pieceTables[move.type][toMove][move.targetField]-pieceTables[move.type][toMove][move.sourceField]);
	position->pieceTableEval = position->pieceTableEval-(1-2*toMove)*pieceTables[move.captureType][1-toMove][move.targetField];
}

inline static void undoKingSideCastling(chessPosition* position, chessMove move) {
	//TODO: remove the ifs
	position->toMove = (playerColor) (1-position->toMove);
	playerColor toMove = position->toMove;

	if(toMove == white){
		position->pieces[toMove] 		     = position->pieces[toMove]^(WHITEKINGSIDECASTLEOCCUPANCYCHANGE);
		position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(WHITEKINGSIDECASTLEROOKMOVE);
		position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(WHITEKINGSIDECASTLEKINGMOVE);
		position->pieceTableEval             = position->pieceTableEval-(-pieceTables[rook][white][7]+pieceTables[rook][white][5]-pieceTables[king][white][4]+pieceTables[king][white][6]);
	} else {
		position->pieces[toMove] 		     = position->pieces[toMove]^(BLACKKINGSIDECASTLEOCCUPANCYCHANGE);
		position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(BLACKKINGSIDECASTLEROOKMOVE);
		position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(BLACKKINGSIDECASTLEKINGMOVE);
		position->pieceTableEval             = position->pieceTableEval-pieceTables[rook][black][63]+pieceTables[rook][black][61]-pieceTables[king][black][60]+pieceTables[king][black][62];
	}
}

inline static void undoQueenSideCastling(chessPosition* position, chessMove move) {
	position->toMove = (playerColor) (1-position->toMove);
	playerColor toMove = position->toMove;
	if(toMove == white){
		position->pieces[toMove] 		     = position->pieces[toMove]^(WHITEQUEENSIDECASTLEOCCUPANCYCHANGE);
		position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(WHITEQUEENSIDECASTLEROOKMOVE);
		position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(WHITEQUEENSIDECASTLEQUEENMOVE);
		position->pieceTableEval             = position->pieceTableEval-(-pieceTables[rook][white][0]+pieceTables[rook][white][3]-pieceTables[king][white][4]+pieceTables[king][white][2]);
	} else {
		position->pieces[toMove] 		     = position->pieces[toMove]^(BLACKQUEENSIDECASTLEOCCUPANCYCHANGE);
		position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(BLACKQUEENSIDECASTLEROOKMOVE);
		position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(BLACKQUEENSIDECASTLEQUEENMOVE);
		position->pieceTableEval             = position->pieceTableEval-pieceTables[rook][black][56]+pieceTables[rook][black][59]-pieceTables[king][black][60]+pieceTables[king][black][58];
	}
}

inline static void undoEnPassant(chessPosition* position, chessMove move) {
	position->toMove = (playerColor) (1-position->toMove);
	playerColor toMove 									= position->toMove;
	position->pieces[toMove] 							= position->pieces[toMove]^move.move;
	position->pieceTables[toMove][pawn] 			    = position->pieceTables[toMove][pawn]^move.move;
	uint16_t shift 										= (toMove? move.targetField+8: move.targetField-8);
	position->pieceTables[1-toMove][pawn] 				= position->pieceTables[1-toMove][pawn] ^ BIT64(shift);
	position->pieces[1-toMove] 							= position->pieces[1-toMove] ^ BIT64(shift);
	position->pieceTableEval 							= position->pieceTableEval-(1-2*toMove)*(pieceTables[pawn][toMove][move.targetField]-pieceTables[pawn][toMove][move.sourceField]);
	position->pieceTableEval 							= position->pieceTableEval-(1-2*toMove)*pieceTables[pawn][1-toMove][shift];


}

inline static void undoPromotion(chessPosition* position, chessMove move, figureType promotedFigure) {
	position->toMove = (playerColor) (1-position->toMove);
	playerColor toMove 									= position->toMove;
	position->pieces[toMove] 							= position->pieces[toMove]^move.move;
	position->pieceTables[toMove][pawn] 				= position->pieceTables[toMove][pawn]^BIT64(move.sourceField);
	position->pieceTables[toMove][promotedFigure] 		= position->pieceTables[toMove][promotedFigure]^BIT64(move.targetField);
	uint64_t isCapture 									= ((none == move.captureType) ? 0:UINT64_MAX);
	position->pieces[1-toMove] 							= (position->pieces[1-toMove] | (isCapture & move.move)) & (~position->pieces[toMove]);
	position->pieceTables[1-toMove][move.captureType] 	= (position->pieceTables[1-toMove][move.captureType] | (isCapture & move.move)) & (~position->pieces[toMove]);
	position->pieceTableEval 							= position->pieceTableEval-(1-2*toMove)*(pieceTables[promotedFigure][toMove][move.targetField]-pieceTables[pawn][toMove][move.sourceField]);
	position->pieceTableEval 							= position->pieceTableEval-(1-2*toMove)*pieceTables[move.captureType][1-toMove][move.targetField];
	position->figureEval     							= position->figureEval-(1-2*toMove)*(figureValues[promotedFigure]-figureValues[pawn]);
}


void undoMove(chessPosition* position) {

	if(position->madeMoves.length == 0){
		std::cout << "Nothing to undo" << std::endl;
		return;
	}


	chessMove move = position->madeMoves.pop();
	uint16_t flags = position->castlingAndEpStack.pop();
	position->figureEval     = position->figureEval+(1-2*position->toMove)*figureValues[move.captureType];
	position->castlingRights = flags & 0xFF;
	position->enPassantFile = ((flags >> 8)) & 0xFF;


	switch(move.type){
		case pawnMove:
		case knightMove:
		case bishopMove:
		case rookMove:
		case queenMove:
		case kingMove:
				{
				undoNormalMove(position, move);
				break;
				}
		case castlingKingside:
			{
				undoKingSideCastling(position, move);
				break;
			}
			case castlingQueenside:
			{
				undoQueenSideCastling(position, move);
				break;
			}
			case enpassant: {
				undoEnPassant(position, move);
				break;
			}
			case promotionQueen:   {
				undoPromotion(position, move, queen);
				break;
			}
			default:
				break;
	}

	#ifdef DEBUG

	int16_t eval = calcFigureEvaluation(position);

	if(eval != position->figureEval){
		std::cout << "Figure evaluation is wrong after undoMove!" << std::endl;
	}

	int16_t pieceTableEval = calcPieceTableValue(position);

	if(pieceTableEval != position->pieceTableEval){
		std::cout << "Piece table evaluation is wrong after undoMove!" << std::endl;
	}
	#endif
}
