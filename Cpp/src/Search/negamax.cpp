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
#include "search.hpp"
#include <hashTables/hashTables.hpp>

static uint32_t nodes = 0;

extern uint16_t moveOrderingHashTable[];

static int32_t indices[150] = {0};

void resetIndices(){
	memset(indices, 0, 150*sizeof(int32_t));
}

int32_t* getIndexCounts(){
	return indices;
}

void resetNodes(){
	nodes = 0;
}

uint32_t getNodes(){
	return nodes;
}


static chessMove buffer[1500];

int32_t negamax(chessPosition* position, uint16_t depth, int32_t alpha, int32_t beta, chessMove* bestMove) {

	int16_t bestIndex = -1;
	if(depth == 0) {
		//return (1-2*position->toMove)*(position->figureEval+position->pieceTableEval);
		return negamaxQuiescence(position, alpha, beta, 0);
	}

	vdt_vector<chessMove> moves = vdt_vector<chessMove>(buffer+depth*100,100);
	generateAllMoves(&moves, position);
	bool isInCheck = orderStandardMoves(position, &moves);

	//TODO: we currently detect stalemate as checkmate!!
	bool legalMovesAvailable = false;
	for(uint16_t ind=0; ind < moves.length; ind++){
		if(moves[ind].sortEval < -10000){
			break;
		}

		makeMove(&moves[ind], position);
		uint16_t kingField = findLSB( position->pieceTables[1- position->toMove][king]);

		if(isFieldAttacked( position,  position->toMove, kingField)){

		} else {
			nodes++;
			legalMovesAvailable = true;
			chessMove mv;
			int32_t value = -negamax(position, depth-1, -beta, -alpha, &mv);

			if(value > alpha){
				alpha = value;
				*bestMove = moves[ind];
				bestIndex = ind;
			}



		}
		undoMove(position);

		/*if(depth == 8) {
			std::cout << "Searched " << moveToString(moves[ind], *position) << " with eval " << alpha << " and sortEval " << moves[ind].sortEval << std::endl;
		}*/
		if(alpha >= beta) {
			uint32_t hashIndex = position->zobristHash & HASHSIZE;
			moveOrderingHashTable[hashIndex] = (bestMove->sourceField | (bestMove->targetField << 8));

			if(bestIndex != -1){
				indices[bestIndex]++;
			}

			//moves.free_array();
			return beta;
		}
	}

	if(bestIndex != -1){
		indices[bestIndex]++;
	}
	uint32_t hashIndex = position->zobristHash & HASHSIZE;
	moveOrderingHashTable[hashIndex] = (bestMove->sourceField | (bestMove->targetField << 8));
	if(!legalMovesAvailable){
		if(isInCheck){
			return -99000-depth;
		} else {
			return 0;
		}
	}
	//moves.free_array();
	return alpha;



}
