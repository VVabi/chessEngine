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
}


void undoMove(chessMove* move, chessPosition* position) {
	//not for castling, promotion and e.p. !
	position->toMove = (playerColor) (1-position->toMove);
	playerColor toMove 									= position->toMove;
	position->pieces[toMove] 							= position->pieces[toMove]^move->move;
	position->pieceTables[toMove][move->type] 			= position->pieceTables[toMove][move->type]^move->move;
	if(move->captureType != none) {
		position->pieces[1-toMove] 							= (position->pieces[1-toMove] | move->move) & (~position->pieces[toMove]);
		position->pieceTables[1-toMove][move->captureType] 	= (position->pieceTables[1-toMove][move->captureType] | move->move) & (~position->pieces[toMove]);
	}
}
