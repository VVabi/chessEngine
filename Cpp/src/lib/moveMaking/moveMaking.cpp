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
#include <hashTables/hashTables.hpp>
#include <assert.h>
#include <parameters/parameters.hpp>

extern int32_t completePieceTables[7][2][64];
extern uint64_t zobristHash[7][2][64];
extern uint64_t movingSideHash[2];
extern uint64_t castlingHash[16];
extern uint64_t enpassantHash[9];
extern uint16_t repetitionData[16384];

void makeNullMove(chessPosition* position){
	position->data.hash = position->zobristHash;
	position->dataStack.add(&position->data);
	position->zobristHash = position->zobristHash^enpassantHash[position->data.enPassantFile];
	position->data.enPassantFile  = 8;
	position->zobristHash = position->zobristHash^enpassantHash[position->data.enPassantFile];
	position->data.fiftyMoveRuleCounter = 0;
	position->zobristHash = position->zobristHash^movingSideHash[0];
	position->toMove = (playerColor) (1-position->toMove);
	chessMove move;
	move.move = 0;

	if(position->madeMoves.length > 0) {
		assert(position->madeMoves[position->madeMoves.length-1].move != 0);
	}
	position->madeMoves.add(&move);
}

inline static void makeNormalMove(chessMove* move, chessPosition* position) {
	playerColor toMove 									= position->toMove;
	position->pieces[toMove] 							= position->pieces[toMove]^move->move;
	position->pieces[1-toMove] 							= position->pieces[1-toMove] & (~move->move);
	position->pieceTables[toMove][move->type] 			= position->pieceTables[toMove][move->type]^move->move;
	position->pieceTables[1-toMove][move->captureType] 	= position->pieceTables[1-toMove][move->captureType] & (~move->move);

	position->pieceTableEval = position->pieceTableEval+(1-2*toMove)*(completePieceTables[move->type][toMove][move->targetField]-completePieceTables[move->type][toMove][move->sourceField]);
	position->pieceTableEval = position->pieceTableEval+(1-2*toMove)*completePieceTables[move->captureType][1-toMove][move->targetField];

	position->zobristHash    = position->zobristHash^zobristHash[move->type][toMove][move->targetField]^zobristHash[move->type][toMove][move->sourceField]^zobristHash[move->captureType][1-toMove][move->targetField];
}


//TODO: get rid of the ifs
inline static void makeKingSideCastle(chessMove* move, chessPosition* position) {
	playerColor toMove = position->toMove;

	if(toMove == white){
		position->pieces[toMove] 		     = position->pieces[toMove]^(WHITEKINGSIDECASTLEOCCUPANCYCHANGE);
		position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(WHITEKINGSIDECASTLEROOKMOVE);
		position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(WHITEKINGSIDECASTLEKINGMOVE);
		position->pieceTableEval             = position->pieceTableEval-completePieceTables[rook][white][7]+completePieceTables[rook][white][5]-completePieceTables[king][white][4]+completePieceTables[king][white][6];
		position->zobristHash                = position->zobristHash^zobristHash[rook][white][7]^zobristHash[rook][white][5]^zobristHash[king][white][4]^zobristHash[king][white][6];

	} else {
		position->pieces[toMove] 		     = position->pieces[toMove]^(BLACKKINGSIDECASTLEOCCUPANCYCHANGE);
		position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(BLACKKINGSIDECASTLEROOKMOVE);
		position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(BLACKKINGSIDECASTLEKINGMOVE);
		position->pieceTableEval             = position->pieceTableEval-(-completePieceTables[rook][black][63]+completePieceTables[rook][black][61]-completePieceTables[king][black][60]+completePieceTables[king][black][62]);
		position->zobristHash                = position->zobristHash^zobristHash[rook][black][63]^zobristHash[rook][black][61]^zobristHash[king][black][60]^zobristHash[king][black][62];
	}
}


inline static void makeQueenSideCastle(chessMove* move, chessPosition* position) {
	playerColor toMove = position->toMove;
	if(toMove == white){
		position->pieces[toMove] 		     = position->pieces[toMove]^(WHITEQUEENSIDECASTLEOCCUPANCYCHANGE);
		position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(WHITEQUEENSIDECASTLEROOKMOVE);
		position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(WHITEQUEENSIDECASTLEQUEENMOVE);
		position->pieceTableEval             = position->pieceTableEval-completePieceTables[rook][white][0]+completePieceTables[rook][white][3]-completePieceTables[king][white][4]+completePieceTables[king][white][2];
		position->zobristHash                = position->zobristHash^zobristHash[rook][white][0]^zobristHash[rook][white][3]^zobristHash[king][white][4]^zobristHash[king][white][2];
	} else {
		position->pieces[toMove] 		     = position->pieces[toMove]^(BLACKQUEENSIDECASTLEOCCUPANCYCHANGE);
		position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(BLACKQUEENSIDECASTLEROOKMOVE);
		position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(BLACKQUEENSIDECASTLEQUEENMOVE);
		position->pieceTableEval             = position->pieceTableEval-(-completePieceTables[rook][black][56]+completePieceTables[rook][black][59]-completePieceTables[king][black][60]+completePieceTables[king][black][58]);
		position->zobristHash                = position->zobristHash^zobristHash[rook][black][56]^zobristHash[rook][black][59]^zobristHash[king][black][60]^zobristHash[king][black][58];
	}

}

inline static void makeEnPassant(chessMove* move, chessPosition* position) {
	playerColor toMove 									= position->toMove;
	position->pieces[toMove] 							= position->pieces[toMove]^move->move;
	position->pieceTables[toMove][pawn] 			    = position->pieceTables[toMove][pawn]^move->move;
	uint16_t shift 										= (toMove? move->targetField+8: move->targetField-8);
	position->pieceTables[1-toMove][move->captureType] 	= position->pieceTables[1-toMove][pawn] ^ BIT64(shift);
	position->pieces[1-toMove] 							= position->pieces[1-toMove] ^ BIT64(shift);
	position->pieceTableEval = position->pieceTableEval+(1-2*toMove)*(completePieceTables[pawn][toMove][move->targetField]-completePieceTables[pawn][toMove][move->sourceField]);
	position->pieceTableEval = position->pieceTableEval+(1-2*toMove)*completePieceTables[pawn][1-toMove][shift];

	position->zobristHash = position->zobristHash^zobristHash[pawn][toMove][move->targetField]^zobristHash[pawn][toMove][move->sourceField]^zobristHash[pawn][1-toMove][shift];
	//position->totalFigureEval     = position->totalFigureEval-figureValues[pawn];
}


inline static void makePromotion(chessMove* move, chessPosition* position, figureType promotedFigure) {
	playerColor toMove 									= position->toMove;
	position->pieces[toMove] 							= position->pieces[toMove]^move->move;
	position->pieces[1-toMove] 							= position->pieces[1-toMove] & (~move->move);
	position->pieceTables[toMove][pawn] 				= position->pieceTables[toMove][pawn]  ^ BIT64(move->sourceField);
	position->pieceTables[toMove][promotedFigure] 		= position->pieceTables[toMove][promotedFigure] ^ BIT64(move->targetField);
	position->pieceTables[1-toMove][move->captureType] 	= position->pieceTables[1-toMove][move->captureType] & (~move->move);

	position->pieceTableEval = position->pieceTableEval+(1-2*toMove)*(completePieceTables[promotedFigure][toMove][move->targetField]-completePieceTables[pawn][toMove][move->sourceField]);
	position->pieceTableEval = position->pieceTableEval+(1-2*toMove)*completePieceTables[move->captureType][1-toMove][move->targetField];
	position->zobristHash = position->zobristHash^zobristHash[promotedFigure][toMove][move->targetField]^zobristHash[pawn][toMove][move->sourceField]^zobristHash[move->captureType][1-toMove][move->targetField];
	const evalParameters* evalPars = getEvalParameters();
	position->figureEval     = position->figureEval+(1-2*toMove)*(evalPars->figureValues[promotedFigure]-evalPars->figureValues[pawn]);
	position->totalFigureEval     = position->totalFigureEval+(evalPars->figureValues[promotedFigure]-evalPars->figureValues[pawn]);
}

void makeMove(chessMove* move, chessPosition* position) {


	assert(move->captureType != king);
	position->data.hash = position->zobristHash;
	position->dataStack.add(&position->data);
	position->data.fiftyMoveRuleCounter = position->data.fiftyMoveRuleCounter+1;

	bool isPawn = (move->type == pawnMove) || (move->type == enpassant) || (move->type == promotionQueen) || (move->type == promotionRook) || (move->type == promotionBishop) || (move->type == promotionKnight);
	bool isCapture = (move->captureType != none);

	if(isPawn || isCapture){
		position->data.fiftyMoveRuleCounter = 0;
	}

	uint8_t castlingRights = position->data.castlingRights;
	position->zobristHash = position->zobristHash^castlingHash[position->data.castlingRights];
	castlingRights = (move->move & WHITEKINGSIDECASTLEMASK  ? (castlingRights & 14):castlingRights);
	castlingRights = (move->move & WHITEQUEENSIDECASTLEMASK ? (castlingRights & 13):castlingRights);
	castlingRights = (move->move & BLACKKINGSIDECASTLEMASK  ? (castlingRights & 11):castlingRights);
	castlingRights = (move->move & BLACKQUEENSIDECASTLEMASK ? (castlingRights &  7):castlingRights);
	position->data.castlingRights = castlingRights;
	position->zobristHash = position->zobristHash^castlingHash[position->data.castlingRights];
	const evalParameters* evalPars = getEvalParameters();
	position->figureEval     = position->figureEval+(1-2*position->toMove)*evalPars->figureValues[move->captureType];
	position->totalFigureEval     = position->totalFigureEval-evalPars->figureValues[move->captureType];
	position->zobristHash = position->zobristHash^enpassantHash[position->data.enPassantFile];
	position->data.enPassantFile  = 8;
	switch(move->type){
		case pawnMove:
			if(((move->targetField-move->sourceField) & 15) == 0) { //pawn went two ahead
				position->data.enPassantFile = FILE(move->targetField);
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
		case promotionRook:
		case promotionBishop:
		case promotionKnight:
		{
			makePromotion(move, position, (figureType) (move->type-8));
			break;
		}
		default:
			break;
	}
	position->zobristHash = position->zobristHash^enpassantHash[position->data.enPassantFile];
	position->madeMoves.add(move);
	position->toMove = (playerColor) (1-position->toMove);
	position->zobristHash = position->zobristHash^movingSideHash[0];
	repetitionData[position->zobristHash & 16383]++;
	#ifdef DEBUG

	debug_incremental_calculations(position);

	#endif
}



