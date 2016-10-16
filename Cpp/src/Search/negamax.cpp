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
#include <lib/Evaluation/evaluation.hpp>
#include <fstream>
#include <userInterface/UIlayer.hpp>


extern uint16_t moveOrderingHashTable[];



static searchDebugData searchCounts;

searchDebugData getSearchData(){
	return searchCounts;
}
void resetSearchData(){
	memset(&searchCounts, 0, sizeof(searchCounts));
}


uint16_t killerMoves[20][2];

static chessMove buffer[2500];

uint16_t repetitionData[16384] = {0};

//std::ofstream logger("/home/vabi/log8.txt");

static inline bool backtrack_position_for_repetition(chessPosition* position) {
	int16_t numMovesToCheck = position->data.fiftyMoveRuleCounter;
	uint64_t current_hash = position->zobristHash;
	assert(((int) position->madeMoves.length)-((int) numMovesToCheck) >= 0);
	assert(position->madeMoves.length == position->dataStack.length);
	for(int16_t ind = ((int) position->madeMoves.length-1); ind >= ((int) position->madeMoves.length)-numMovesToCheck; ind--){
		if(position->dataStack[ind].hash == current_hash){
			return true;
		}
	}
	return false;
}



int32_t negamax(chessPosition* position, int16_t depth, int32_t alpha, int32_t beta, chessMove* bestMove) {

	assert(alpha < beta);
	searchCounts.called++;

	if(position->data.fiftyMoveRuleCounter >= 100){
		return 0;
	}

	if(repetitionData[position->zobristHash & 16383] > 1){
		if(backtrack_position_for_repetition(position)){
			searchCounts.threefold_repetitions++;
			return 0;
		} else {
			searchCounts.fake_3fold_repetitions++;
		}
	}

	//remembers the index of best move after sorting
	//-------------------------------------------
	int16_t bestIndex = -1;

	//go to quiescence on depth 0
	//---------------------------
	if(depth <= 0) {
		searchCounts.wentToQuiescence++;
		return negamaxQuiescence(position, alpha, beta, 0);
	}

	//futility pruning
	//-----------------
	if(depth == 1){
		searchCounts.futility_tried++;
		int32_t base = evaluation(position, alpha-151, beta);
		if(base+150 < alpha){
			searchCounts.futility_successful++;
			//in this case, trying a silent move is pointless.
			//std::cout << "Successful futility pruning" << std::endl;
			return  negamaxQuiescence(position, alpha, beta, 0);
		}
	}

	vdt_vector<chessMove> moves = vdt_vector<chessMove>(buffer+depth*100,100);
	generateAllMoves(&moves, position);
	bool isInCheck = orderStandardMoves(position, &moves, depth);


	bool legalMovesAvailable = false;
	bool foundGoodMove = false;
	searchCounts.wentToSearch++;
	for(uint16_t ind=0; ind < moves.length; ind++){

		if(ind == 1){
			searchCounts.neededSort++;
			std::sort(moves.data, moves.data+moves.length);
		}

		if(moves[ind].sortEval < -10000){
			break;
		}


		makeMove(&moves[ind], position);
		uint16_t kingField = findLSB( position->pieceTables[1- position->toMove][king]);

		if(isFieldAttacked( position,  position->toMove, kingField)){

		} else {
			searchCounts.nodes[depth]++;
			legalMovesAvailable = true;
			if((depth > 2)) {
				//check that the side to move is not in check.
				uint16_t kingField2 = findLSB( position->pieceTables[position->toMove][king]);

				if(!isFieldAttacked( position, (playerColor) (1-position->toMove), kingField2)){
					makeNullMove(position);
					chessMove mv;
					int32_t value = negamax(position, depth-1-2, alpha, alpha+1, &mv);
					undoNullMove(position);
					if(value < alpha+1) {
						undoMove(position);
						//std::cout << "nullmove pruning successful!" << std::endl;
						continue;
					}
				}
			}


			chessMove mv;

			//PVSearch, currently a net LOSS for us
			//-------------------------------------------------
			/*if((ind > 2) && (depth > 2)) {
				int32_t value = -negamax(position, depth-1, -alpha-1, -alpha, &mv);
				if(value < alpha+1){
					undoMove(position);
					continue;
				}
			}*/
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
				searchCounts.bestIndex[depth][bestIndex]++;
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
		searchCounts.bestIndex[depth][bestIndex]++;
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
