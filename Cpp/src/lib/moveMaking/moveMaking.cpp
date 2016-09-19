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

void makeMove(chessMove* move, chessPosition* position) {

	uint16_t to_push = position->castlingRights | (((uint16_t) position->enPassantField) << 8);
	position->castlingAndEpStack.add(&to_push);
	position->castlingRights = (move->move & WHITEKINGSIDECASTLEMASK  ? (position->castlingRights & 14):position->castlingRights);
	position->castlingRights = (move->move & WHITEQUEENSIDECASTLEMASK ? (position->castlingRights & 13):position->castlingRights);
	position->castlingRights = (move->move & BLACKKINGSIDECASTLEMASK  ? (position->castlingRights & 11):position->castlingRights);
	position->castlingRights = (move->move & BLACKQUEENSIDECASTLEMASK ? (position->castlingRights &  7):position->castlingRights);

	position->figureEval     = position->figureEval+(1-2*position->toMove)*figureValues[move->captureType];

	switch(move->type){
		case pawnMove:
		case knightMove:
		case bishopMove:
		case rookMove:
		case queenMove:
		case kingMove:
		{
			playerColor toMove 									= position->toMove;
			position->pieces[toMove] 							= position->pieces[toMove]^move->move;
			position->pieces[1-toMove] 							= position->pieces[1-toMove] & (~move->move);
			position->pieceTables[toMove][move->type] 			= position->pieceTables[toMove][move->type]^move->move;
			position->pieceTables[1-toMove][move->captureType] 	= position->pieceTables[1-toMove][move->captureType] & (~move->move);

			position->pieceTableEval = position->pieceTableEval+(1-2*toMove)*(pieceTables[move->type][toMove][move->targetField]-pieceTables[move->type][toMove][move->sourceField]);
			position->pieceTableEval = position->pieceTableEval+(1-2*toMove)*pieceTables[move->captureType][1-toMove][move->targetField];
			position->toMove = (playerColor) (1-position->toMove);
			//TODO: add figure eval
			//TODO: Add hash
			break;
		}

		//TODO: get rid of the ifs
		case castlingKingside:
		{
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
			break;
		}
		case castlingQueenside:
		{
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
			break;
		}

		case enpassant:
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


void undoMove(chessPosition* position) {

	if(position->madeMoves.length == 0){
		std::cout << "Nothing to undo" << std::endl;
		return;
	}


	chessMove move = position->madeMoves.pop();
	uint16_t flags = position->castlingAndEpStack.pop();
	position->figureEval     = position->figureEval+(1-2*position->toMove)*figureValues[move.captureType];
	position->castlingRights = flags & 0xFF;
	position->enPassantField = ((flags >> 8)) & 0xFF;


	switch(move.type){
		case pawnMove:
		case knightMove:
		case bishopMove:
		case rookMove:
		case queenMove:
		case kingMove:
				{
				position->toMove = (playerColor) (1-position->toMove);
				playerColor toMove 									= position->toMove;
				position->pieces[toMove] 							= position->pieces[toMove]^move.move;
				position->pieceTables[toMove][move.type] 			= position->pieceTables[toMove][move.type]^move.move;
				uint64_t isCapture = ((none == move.captureType) ? 0:UINT64_MAX);
				position->pieces[1-toMove] 							= (position->pieces[1-toMove] | (isCapture & move.move)) & (~position->pieces[toMove]);
				position->pieceTables[1-toMove][move.captureType] 	= (position->pieceTables[1-toMove][move.captureType] | (isCapture & move.move)) & (~position->pieces[toMove]);
				position->pieceTableEval = position->pieceTableEval-(1-2*toMove)*(pieceTables[move.type][toMove][move.targetField]-pieceTables[move.type][toMove][move.sourceField]);
				position->pieceTableEval = position->pieceTableEval-(1-2*toMove)*pieceTables[move.captureType][1-toMove][move.targetField];
				break;
				}
		case castlingKingside:
			{
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

				break;
			}
			case castlingQueenside:
			{
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
