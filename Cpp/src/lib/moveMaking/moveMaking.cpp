/*
 * moveMaking.cpp
 *
 *  Created on: Sep 2, 2016
 *      Author: vabi
 */

#include <lib/basics.hpp>


void makeMove(chessMove* move, chessPosition* position) {
	//not for castling, promotion and e.p. !
	playerColor toMove 									= position->toMove;
	position->pieces[toMove] 							= position->pieces[toMove]^move->move;
	position->pieces[1-toMove] 							= position->pieces[1-toMove] & (~move->move);
	position->pieceTables[toMove][move->type] 			= position->pieceTables[toMove][move->type]^move->move;
	position->pieceTables[1-toMove][move->captureType] 	= position->pieceTables[1-toMove][move->captureType] & (~move->move);
	position->toMove = (playerColor) (1-position->toMove);
	//TODO: add figure eval
	//TODO: Add hash
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
