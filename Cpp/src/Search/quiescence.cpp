/*
 * quiescence.cpp
 *
 *  Created on: Sep 22, 2016
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
#include "search.hpp"
#include <hashTables/hashTables.hpp>
#include <lib/Evaluation/evaluation.hpp>


extern uint16_t moveOrderingHashTable[];


static uint32_t nodes = 0;

void resetQuiescenceNodes(){
	nodes = 0;
}

uint32_t getQuiescenceNodes(){
	return nodes;
}

static chessMove buffer[32*50];


int32_t negamaxQuiescence(chessPosition* position, int32_t alpha, int32_t beta, uint16_t depth) {

	int32_t baseEval = evaluation(position);

	if(baseEval > alpha){
		alpha = baseEval;
	}
	if(alpha >= beta) {
		return beta;
	}
	chessMove bestMove;
	bestMove.sourceField = 0;
	bestMove.targetField = 0;
	vdt_vector<chessMove> moves = vdt_vector<chessMove>(buffer+depth*50, 50);
	generateAllCaptureMoves(&moves, position);
	orderCaptureMoves(position, &moves);
	for(uint16_t ind=0; ind < moves.length; ind++){
		makeMove(&moves[ind], position);
		uint16_t kingField = findLSB( position->pieceTables[1- position->toMove][king]);

		if(isFieldAttacked( position,  position->toMove, kingField)){

		} else {
			nodes++;


			int32_t value = -negamaxQuiescence(position, -beta, -alpha, depth+1);

			if(value > alpha){
				alpha = value;
				bestMove = moves[ind];

			}



		}
		undoMove(position);
		if(alpha >= beta) {
			/*uint32_t hashIndex = position->zobristHash & HASHSIZE;
			moveOrderingHashTable[hashIndex] = (bestMove.sourceField | (bestMove.targetField << 8));*/
			//moves.free_array();
			return beta;
		}
	}


	//moves.free_array();
	return alpha;



}


