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


extern hashEntry moveOrderingHashTable[];
extern uint32_t historyTable[2][64][64];
extern uint8_t searchId;

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

static inline bool check_nullmove(chessPosition* position, int16_t depth, int32_t beta){


	if(beta > 10000){ //TODO: more dynamic condition here?
		return false;
	}

	int32_t eval = evaluation(position, beta-1, beta);

	if(eval < beta){
		return false; //no point in trying nullmove when the current evaluation is already worse than beta
	}

	uint16_t kingField = findLSB(position->pieceTables[position->toMove][king]);
	if(isFieldAttacked( position, (playerColor) (1-position->toMove), kingField)){
		return false; //we are in check, nullmove is regi-suicide
	}

	makeNullMove(position);
	chessMove mv;
	searchCounts.nullMovePruningTried++;
	int32_t value = -negamax(position, depth-3, -beta, -beta+1, &mv, false);
	undoNullMove(position);
	if(value < beta) {
		return false; //didnt manage a cutoff :(
	}
	searchCounts.nullMovePruningSuccessful++;
	return true;
}

int16_t negamax(chessPosition* position, int16_t depth, int16_t alpha, int16_t beta, chessMove* bestMove, bool allowNullMove, bool doHashProbe) {



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

	uint32_t hashIndex2 = position->zobristHash & HASHSIZE;
	hashEntry hashVal    = moveOrderingHashTable[hashIndex2];
	uint32_t zobristHigher = (uint32_t) (position->zobristHash >> 32);
	uint16_t zobristLower  = (uint16_t) (((uint32_t) (position->zobristHash & 0xFFFFFFFF)) >> 16);
	if(doHashProbe){
		if((zobristHigher == hashVal.hashHighBits) && (zobristLower == hashVal.hashLower)) { //TODO: assign bestMove - this can blow up in our face easily TODO: add proper checkmate handling
			int16_t oldEval  = hashVal.eval;
			if((depth <= hashVal.depth) && (oldEval > -10000) && (oldEval < 10000)){
				int16_t oldAlpha = hashVal.alpha;
				int16_t oldBeta  = hashVal.beta;
				if((oldAlpha < oldEval) && (oldEval >= beta)){ //oldEval is actually computed (i.e. not fail low) AND we get a beta cutoff
					return beta;
				}
				else if((oldAlpha <= alpha) && (oldEval <= oldAlpha)){
					return alpha; //node will fail low
				}
				else if((oldEval < oldBeta) && (oldEval > oldAlpha)){ //TODO: this condition can be vastly improved
					return oldEval; //TODO: we are typically fail-hard, so should be fail-hard here as well - but that cannot cause the bug I'm looking for I believe
				}
			}
		}
	}
	//go to quiescence on depth 0
	//---------------------------
	if(depth <= 0) {
		searchCounts.wentToQuiescence++;
		return negamaxQuiescence(position, alpha, beta, 0);
	}

	if((depth >= 2) && allowNullMove){
		if(check_nullmove(position, depth, beta)){
			return beta;
		}
	}
	const int16_t origAlpha = alpha;
	const int16_t origBeta = beta;
	//remembers the index of best move after sorting
	//-------------------------------------------
	int16_t bestIndex = -1;



	//futility pruning
	//-----------------
	if(depth == 1){
		searchCounts.futility_tried++;
		int32_t base = evaluation(position, alpha-151, alpha);
		if(base+150 < alpha){
			searchCounts.futility_successful++;
			//in this case, trying a silent move is pointless.
			//std::cout << "Successful futility pruning" << std::endl;
			return  negamaxQuiescence(position, alpha, beta, 0);
		}
	}

	vdt_vector<chessMove> moves = vdt_vector<chessMove>(buffer+depth*100,100);
	generateAllMoves(&moves, position);
	bool isInCheck = orderStandardMoves(position, &moves, depth, hashVal.bestMove);


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
			searchCounts.totalNodes++;
			legalMovesAvailable = true;
			chessMove mv;

			//PVSearch, currently a small gain for us with the > 5
			//-------------------------------------------------
			if((ind > 5) && (depth > 2)) {
				int32_t value = -negamax(position, depth-1, -alpha-1, -alpha, &mv);
				if(value < alpha+1){
					undoMove(position);
					continue;
				}
			}

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
			if((depth >= hashVal.depth) || (searchId != hashVal.searchId)) {
				uint32_t hashIndex = position->zobristHash & HASHSIZE;
				hashEntry entry;
				entry.alpha = origAlpha;
				entry.beta  = origBeta;
				entry.eval  = origBeta; //beta cutoff
				assert(beta == origBeta);
				entry.depth = depth;
				entry.searchId = searchId;
				entry.bestMove = (bestMove->sourceField | (bestMove->targetField << 8));
				entry.hashHighBits = (uint32_t) (position->zobristHash >> 32);
				entry.hashLower    = (uint16_t) (((uint32_t) (position->zobristHash & 0xFFFFFFFF)) >> 16);
				moveOrderingHashTable[hashIndex] = entry;
			}
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
		historyTable[position->toMove][bestMove->sourceField][bestMove->targetField] += 2*depth*depth;
		if(historyTable[position->toMove][bestMove->sourceField][bestMove->targetField] > HISTORY_CUTOFF){
			//std::cout << "Rescaling history table " << std::endl;
			rescaleHistoryTable();
		}
		for(uint16_t cnt=0; cnt < bestIndex; cnt++){
			chessMove mv = moves[cnt];
			if(mv.captureType == none){
				historyTable[position->toMove][bestMove->sourceField][bestMove->targetField] -= depth*depth;
				if(historyTable[position->toMove][bestMove->sourceField][bestMove->targetField] < -HISTORY_CUTOFF){
					rescaleHistoryTable();
				}
			}
		}
		searchCounts.bestIndex[depth][bestIndex]++;
	}

	if(!legalMovesAvailable){
		if(isInCheck){
			alpha = -30000-depth;
		} else {
			alpha = 0;
		}
	}

	uint32_t hashIndex = position->zobristHash & HASHSIZE;
	if(foundGoodMove) {
		if((depth >= hashVal.depth) || (searchId != hashVal.searchId)) {
			hashEntry entry;
			entry.alpha = origAlpha;
			entry.beta  = origBeta;
			entry.eval  = alpha;
			entry.depth = depth;
			entry.searchId = searchId;
			entry.bestMove = (bestMove->sourceField | (bestMove->targetField << 8));
			entry.hashHighBits = (uint32_t) (position->zobristHash >> 32);
			entry.hashLower    = (uint16_t) (((uint32_t) (position->zobristHash & 0xFFFFFFFF)) >> 16);
			moveOrderingHashTable[hashIndex] = entry;
		}
	} else { //we failed low, remember as well
		if((depth >= hashVal.depth) || (searchId != hashVal.searchId)) {
			hashEntry entry;
			entry.alpha = origAlpha;
			entry.beta  = origBeta;
			entry.eval  = alpha;
			entry.depth = depth;
			assert(entry.depth == depth);
			entry.searchId = searchId;
			entry.bestMove = 0;
			entry.hashHighBits = (uint32_t) (position->zobristHash >> 32);
			entry.hashLower    = (uint16_t) (((uint32_t) (position->zobristHash & 0xFFFFFFFF)) >> 16);
			moveOrderingHashTable[hashIndex] = entry;
		}
	}

	//moves.free_array();
	return alpha;
}
