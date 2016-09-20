/*
 * negamax.cpp
 *
 *  Created on: Sep 20, 2016
 *      Author: vabi
 */

#include <iostream>
#include <lib/basics.hpp>
#include <lib/moveMaking/moveMaking.hpp>
#include <tests/tests.hpp>
#include <DataTypes/vdt_vector.hpp>
#include <lib/moveGeneration/moveGeneration.hpp>
#include <lib/Attacks/attacks.hpp>
#include <lib/bitfiddling.h>

static uint32_t nodes = 0;

void resetNodes(){
	nodes = 0;
}

uint32_t getNodes(){
	return nodes;
}


int32_t negamax(chessPosition* position, uint16_t depth, int32_t alpha, int32_t beta, chessMove* bestMove) {


	if(depth == 0) {
		return (1-2*position->toMove)*(position->figureEval+position->pieceTableEval);
	}

	vdt_vector<chessMove> moves = vdt_vector<chessMove>(100);
	generateAllMoves(&moves, position);

	bool isMate = true;
	for(uint16_t ind=0; ind < moves.length; ind++){
		makeMove(&moves[ind], position);
		uint16_t kingField = findLSB( position->pieceTables[1- position->toMove][king]);

		if(isFieldAttacked( position,  position->toMove, kingField)){

		} else {
			nodes++;
			isMate = false;
			chessMove mv;
			int32_t value = -negamax(position, depth-1, -beta, -alpha, &mv);

			if(value > alpha){
				alpha = value;
				*bestMove = moves[ind];
			}



		}
		undoMove(position);
		if(alpha >= beta) {
			return beta;
		}
	}

	if(isMate){
		return -100000+depth;
	}
	return alpha;



}
