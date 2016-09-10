/*
 * moveMaking.cpp
 *
 *  Created on: Sep 2, 2016
 *      Author: vabi
 */

#include <lib/basics.hpp>
#include <lib/Defines/boardParts.hpp>


void makeMove(chessMove* move, chessPosition* position) {
	//not for promotion and e.p. !
	position->castlingRights = (move->move & WHITEKINGSIDECASTLEMASK  ? (position->castlingRights & 14):position->castlingRights);
	position->castlingRights = (move->move & WHITEQUEENSIDECASTLEMASK ? (position->castlingRights & 13):position->castlingRights);
	position->castlingRights = (move->move & BLACKKINGSIDECASTLEMASK  ? (position->castlingRights & 11):position->castlingRights);
	position->castlingRights = (move->move & BLACKQUEENSIDECASTLEMASK ? (position->castlingRights &  7):position->castlingRights);

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
			} else {
				position->pieces[toMove] 		     = position->pieces[toMove]^(BLACKKINGSIDECASTLEOCCUPANCYCHANGE);
				position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(BLACKKINGSIDECASTLEROOKMOVE);
				position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(BLACKKINGSIDECASTLEKINGMOVE);
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
			} else {
				position->pieces[toMove] 		     = position->pieces[toMove]^(BLACKQUEENSIDECASTLEOCCUPANCYCHANGE);
				position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(BLACKQUEENSIDECASTLEROOKMOVE);
				position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(BLACKQUEENSIDECASTLEQUEENMOVE);
			}
			position->toMove = (playerColor) (1-position->toMove);
			break;
		}

		case enpassant:
			break;
	}
}


void undoMove(chessMove* move, chessPosition* position) {
	//not for castling, promotion and e.p. !
	position->toMove = (playerColor) (1-position->toMove);
	playerColor toMove 									= position->toMove;
	position->pieces[toMove] 							= position->pieces[toMove]^move->move;
	position->pieceTables[toMove][move->type] 			= position->pieceTables[toMove][move->type]^move->move;

	uint64_t isCapture = ((none == move->captureType) ? 0:UINT64_MAX);
	position->pieces[1-toMove] 							= (position->pieces[1-toMove] | (isCapture & move->move)) & (~position->pieces[toMove]);
	position->pieceTables[1-toMove][move->captureType] 	= (position->pieceTables[1-toMove][move->captureType] | (isCapture & move->move)) & (~position->pieces[toMove]);
	//TODO: add figure eval
	//TODO: Add hash
}
