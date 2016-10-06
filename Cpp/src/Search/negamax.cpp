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
#include <algorithm>
#include <assert.h>

static uint32_t nodes = 0;

extern uint16_t moveOrderingHashTable[];

static int32_t indices[150] = {0};

static uint32_t called = 0;
static uint32_t sortCalled = 0;

uint32_t getCalled(){
	return called;
}

void resetCalled(){
	called = 0;
}

uint32_t getSortCalled(){
	return sortCalled;
}

void resetSortCalled(){
	sortCalled = 0;
}



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


uint16_t killerMoves[20][2];

static chessMove buffer[1500];

int32_t negamax(chessPosition* position, uint16_t depth, int32_t alpha, int32_t beta, chessMove* bestMove) {

	assert(alpha < beta);
	int16_t bestIndex = -1;
	if(depth == 0) {
		//return (1-2*position->toMove)*(position->figureEval+position->pieceTableEval);
		return negamaxQuiescence(position, alpha, beta, 0);
	}

	vdt_vector<chessMove> moves = vdt_vector<chessMove>(buffer+depth*100,100);
	generateAllMoves(&moves, position);
	bool isInCheck = orderStandardMoves(position, &moves, depth);

	called++;
	bool legalMovesAvailable = false;
	bool foundGoodMove = false;
	for(uint16_t ind=0; ind < moves.length; ind++){




		if(ind == 1){
			sortCalled++;
			std::sort(moves.data, moves.data+moves.length);
		}

		if(moves[ind].sortEval < -10000){
			break;
		}



		makeMove(&moves[ind], position);
		uint16_t kingField = findLSB( position->pieceTables[1- position->toMove][king]);

		if(isFieldAttacked( position,  position->toMove, kingField)){

		} else {
			nodes++;
			legalMovesAvailable = true;
			if((depth > 2)) {
				//check that the side to move is not in check.
				uint16_t kingField2 = findLSB( position->pieceTables[position->toMove][king]);

				if(!isFieldAttacked( position, (playerColor) (1-position->toMove), kingField2)){
					makeNullMove(position);
					chessMove mv;
					int32_t value = negamax(position, depth-1-2, alpha-1, alpha, &mv);
					undoNullMove(position);
					if(value < alpha) {
						undoMove(position);
						//std::cout << "nullmove pruning successful!" << std::endl;
						continue;
					}
				}
			}


			chessMove mv;
			int32_t value = -negamax(position, depth-1, -beta, -alpha, &mv);

			if(value > alpha){
				foundGoodMove = true;
				alpha = value;
				*bestMove = moves[ind];
				bestIndex = ind;
			}
		}
		undoMove(position);


		if(alpha >= beta) {
			uint32_t hashIndex = position->zobristHash & HASHSIZE;
			moveOrderingHashTable[hashIndex] = (bestMove->sourceField | (bestMove->targetField << 8));

			if(bestIndex != -1){
				indices[bestIndex]++;
			}

			if(bestMove->captureType == none){
				uint16_t toRemember = (bestMove->sourceField | (bestMove->targetField << 8));

				if ( (killerMoves[depth][0] != toRemember)) {
					killerMoves[depth][1] = killerMoves[depth][0];
					killerMoves[depth][0] = toRemember;
				}


			}

			//moves.free_array();
			return beta;
		}
	}

	if(bestIndex != -1){
		indices[bestIndex]++;
	}
	uint32_t hashIndex = position->zobristHash & HASHSIZE;
	if(foundGoodMove) {
		moveOrderingHashTable[hashIndex] = (bestMove->sourceField | (bestMove->targetField << 8));
	}
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
