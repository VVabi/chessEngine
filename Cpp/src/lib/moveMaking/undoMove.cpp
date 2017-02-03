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
#include <hashTables/hashTables.hpp>
#include <assert.h>
#include <userInterface/UIlayer.hpp>
#include <parameters/parameters.hpp>

extern uint64_t zobristHash[7][2][64];
extern uint64_t movingSideHash[2];
extern int32_t completePieceTables[7][2][64];
extern uint16_t repetitionData[16384];
extern uint64_t castlingHash[16];
extern uint64_t enpassantHash[9];

void undoNullMove(chessPosition* position) {
	position->zobristHash = position->zobristHash^enpassantHash[position->data.enPassantFile];
	position->data = position->dataStack.pop();
	position->zobristHash = position->zobristHash^movingSideHash[0];
	position->toMove = (playerColor) (1-position->toMove);
	position->zobristHash = position->zobristHash^enpassantHash[position->data.enPassantFile];
	position->madeMoves.pop();
}

inline static void undoNormalMove(chessPosition* position, chessMove move) {
	position->toMove = (playerColor) (1-position->toMove);
	playerColor toMove 									= position->toMove;
	position->pieces[toMove] 							= position->pieces[toMove]^move.move;
	position->pieceTables[toMove][move.type] 			= position->pieceTables[toMove][move.type]^move.move;
	uint64_t isCapture = ((none == move.captureType) ? 0:UINT64_MAX);
	position->pieces[1-toMove] 							= (position->pieces[1-toMove] | (isCapture & move.move)) & (~position->pieces[toMove]);
	position->pieceTables[1-toMove][move.captureType] 	= (position->pieceTables[1-toMove][move.captureType] | (isCapture & move.move)) & (~position->pieces[toMove]);
	position->pieceTableEval = position->pieceTableEval-(1-2*toMove)*(completePieceTables[move.type][toMove][move.targetField]-completePieceTables[move.type][toMove][move.sourceField]);
	position->pieceTableEval = position->pieceTableEval-(1-2*toMove)*completePieceTables[move.captureType][1-toMove][move.targetField];
	position->zobristHash    = position->zobristHash^zobristHash[move.type][toMove][move.targetField]^zobristHash[move.type][toMove][move.sourceField]^zobristHash[move.captureType][1-toMove][move.targetField];
}

inline static void undoKingSideCastling(chessPosition* position, chessMove move) {
	//TODO: remove the ifs
	position->toMove = (playerColor) (1-position->toMove);
	playerColor toMove = position->toMove;

	if(toMove == white){
		position->pieces[toMove] 		     = position->pieces[toMove]^(WHITEKINGSIDECASTLEOCCUPANCYCHANGE);
		position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(WHITEKINGSIDECASTLEROOKMOVE);
		position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(WHITEKINGSIDECASTLEKINGMOVE);
		position->pieceTableEval             = position->pieceTableEval-(-completePieceTables[rook][white][7]+completePieceTables[rook][white][5]-completePieceTables[king][white][4]+completePieceTables[king][white][6]);
		position->zobristHash                = position->zobristHash^zobristHash[rook][white][7]^zobristHash[rook][white][5]^zobristHash[king][white][4]^zobristHash[king][white][6];
	} else {
		position->pieces[toMove] 		     = position->pieces[toMove]^(BLACKKINGSIDECASTLEOCCUPANCYCHANGE);
		position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(BLACKKINGSIDECASTLEROOKMOVE);
		position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(BLACKKINGSIDECASTLEKINGMOVE);
		position->pieceTableEval             = position->pieceTableEval-completePieceTables[rook][black][63]+completePieceTables[rook][black][61]-completePieceTables[king][black][60]+completePieceTables[king][black][62];
		position->zobristHash                = position->zobristHash^zobristHash[rook][black][63]^zobristHash[rook][black][61]^zobristHash[king][black][60]^zobristHash[king][black][62];
	}
}

inline static void undoQueenSideCastling(chessPosition* position, chessMove move) {
	position->toMove = (playerColor) (1-position->toMove);
	playerColor toMove = position->toMove;
	if(toMove == white){
		position->pieces[toMove] 		     = position->pieces[toMove]^(WHITEQUEENSIDECASTLEOCCUPANCYCHANGE);
		position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(WHITEQUEENSIDECASTLEROOKMOVE);
		position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(WHITEQUEENSIDECASTLEQUEENMOVE);
		position->pieceTableEval             = position->pieceTableEval-(-completePieceTables[rook][white][0]+completePieceTables[rook][white][3]-completePieceTables[king][white][4]+completePieceTables[king][white][2]);
		position->zobristHash                = position->zobristHash^zobristHash[rook][white][0]^zobristHash[rook][white][3]^zobristHash[king][white][4]^zobristHash[king][white][2];
	} else {
		position->pieces[toMove] 		     = position->pieces[toMove]^(BLACKQUEENSIDECASTLEOCCUPANCYCHANGE);
		position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(BLACKQUEENSIDECASTLEROOKMOVE);
		position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(BLACKQUEENSIDECASTLEQUEENMOVE);
		position->pieceTableEval             = position->pieceTableEval-completePieceTables[rook][black][56]+completePieceTables[rook][black][59]-completePieceTables[king][black][60]+completePieceTables[king][black][58];
		position->zobristHash                = position->zobristHash^zobristHash[rook][black][56]^zobristHash[rook][black][59]^zobristHash[king][black][60]^zobristHash[king][black][58];
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
	position->pieceTableEval 							= position->pieceTableEval-(1-2*toMove)*(completePieceTables[pawn][toMove][move.targetField]-completePieceTables[pawn][toMove][move.sourceField]);
	position->pieceTableEval 							= position->pieceTableEval-(1-2*toMove)*completePieceTables[pawn][1-toMove][shift];
	position->zobristHash = position->zobristHash^zobristHash[pawn][toMove][move.targetField]^zobristHash[pawn][toMove][move.sourceField]^zobristHash[pawn][1-toMove][shift];
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
	position->pieceTableEval 							= position->pieceTableEval-(1-2*toMove)*(completePieceTables[promotedFigure][toMove][move.targetField]-completePieceTables[pawn][toMove][move.sourceField]);
	position->pieceTableEval 							= position->pieceTableEval-(1-2*toMove)*completePieceTables[move.captureType][1-toMove][move.targetField];
	const evalParameters* evalPars 						= getEvalParameters();
	position->figureEval     							= position->figureEval-(1-2*toMove)*(evalPars->figureValues[promotedFigure]-evalPars->figureValues[pawn]);
	position->totalFigureEval     						= position->totalFigureEval-(evalPars->figureValues[promotedFigure]-evalPars->figureValues[pawn]);
	position->zobristHash = position->zobristHash^zobristHash[promotedFigure][toMove][move.targetField]^zobristHash[pawn][toMove][move.sourceField]^zobristHash[move.captureType][1-toMove][move.targetField];
}


void undoMove(chessPosition* position) {

	if(position->madeMoves.length == 0){
		std::cout << "Nothing to undo" << std::endl;
		return;
	}

	/*if(repetitionData[position->zobristHash & 16383] == 0){
		std::cout << position->madeMoves.length << std::endl;
		std::cout << chessPositionToOutputString(*position) << std::endl;
		std::cout << chessPositionToFenString(*position, false) << std::endl;
		for(uint16_t ind=0; ind < position->madeMoves.length; ind++) {
			std::cout << position->madeMoves[ind].sourceField << " " << position->madeMoves[ind].targetField << std::endl;
		}
		std::cout << "WTF" << std::endl;
	}*/

	assert(repetitionData[position->zobristHash & 16383] != 0);
	repetitionData[position->zobristHash & 16383]--;
	chessMove move 						= position->madeMoves.pop();
	position->zobristHash 				= position->zobristHash^enpassantHash[position->data.enPassantFile];
	position->zobristHash 				= position->zobristHash^castlingHash[position->data.castlingRights];
	position->data 						= position->dataStack.pop();
	position->zobristHash 				= position->zobristHash^castlingHash[position->data.castlingRights];
	position->zobristHash 				= position->zobristHash^enpassantHash[position->data.enPassantFile];
	const evalParameters* evalPars 		= getEvalParameters();
	position->figureEval     			= position->figureEval+(1-2*position->toMove)*evalPars->figureValues[move.captureType];
	position->totalFigureEval     		= position->totalFigureEval+evalPars->figureValues[move.captureType];

	//00000BNR00000PKQ00000NPR00000N0P000000000000000p0qpN0npnN000000kw0000

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
			case promotionQueen:
			case promotionRook:
			case promotionBishop:
			case promotionKnight:
			{
				undoPromotion(position, move, (figureType) (move.type-8));
				break;
			}
			default:
				break;
	}

	position->zobristHash = position->zobristHash^movingSideHash[0];

	if(position->zobristHash != position->data.hash){
		std::cout << chessPositionToOutputString(*position) << std::endl;
		std::cout << "WTF???" << std::endl;
	}

	assert(position->zobristHash == position->data.hash);

	#ifdef DEBUG

	debug_incremental_calculations(position);

	#endif
}
