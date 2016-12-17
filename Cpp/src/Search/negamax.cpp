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

extern uint32_t historyTable[2][64][64];
extern uint8_t searchId;

static searchDebugData searchCounts;

searchDebugData getSearchData(){
	return searchCounts;
}

void resetSearchData(){
	memset(&searchCounts, 0, sizeof(searchCounts));
}

static uint32_t totalTime;
static uint64_t start_ts;

void setTotalTime(uint32_t tTime, uint64_t start){
	totalTime = tTime;
	start_ts = start;
}


uint16_t killerMoves[40][2];

//static chessMove buffer[5000];

static moveStack qmvStack;

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

static uint8_t nullmoveReductions[40] = {0,1,2,2,2,2,2,2,
										 2,2,2,2,2,2,2,2,
										 3,3,3,3,3,3,3,3,
										 3,3,3,3,3,3,3,3,
										 3,3,3,3,3,3,3,3,
};

static inline bool check_nullmove(chessPosition* position, uint16_t* refutationMoveTarget, uint16_t ply, uint16_t max_ply, int16_t depth, int32_t beta){


	if(beta > 10000){ //TODO: more dynamic condition here?
		return false;
	}

	/*if(ply > max_ply-2){
		return false;
	}*/
	int32_t eval = evaluation(position, beta-1, beta);
	if(eval < beta){
		return false; //no point in trying nullmove when the current evaluation is already worse than beta
	}

	makeNullMove(position);
	chessMove mv;
	searchCounts.nullMovePruningTried++;
	int32_t value = -negamax(position, ply+1, max_ply-nullmoveReductions[depth], depth-1-nullmoveReductions[depth], -beta, -beta+1, &mv, false);
	undoNullMove(position);
	if(value < beta) {
		*refutationMoveTarget = mv.targetField;
		return false; //didnt manage a cutoff :(
	}
	searchCounts.nullMovePruningSuccessful++;
	return true;
}

int16_t negamax(chessPosition* position, uint16_t ply, uint16_t max_ply, int16_t depth, int16_t alpha, int16_t beta, chessMove* bestMove, bool allowNullMove, bool doHashProbe){

	if(depth >= 3) { //TODO: be careful here - we may have to reset the stack in quiescence search as well!
		if(get_timestamp()-start_ts >= totalTime){ //TODO: how is this performance wise?
			qmvStack.reset();
			throw timeoutException();
		}
	}
	assert(alpha < beta);
	assert(ply+depth <= max_ply);

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

	hashEntry hashVal      = getHashTableEntry(position->zobristHash);

	uint32_t zobristHigher = (uint32_t) (position->zobristHash >> 32);
	uint16_t zobristLower  = (uint16_t) (((uint32_t) (position->zobristHash & 0xFFFFFFFF)) >> 16);
	if(doHashProbe){ //TODO: we should check whether another move leads to 3fold rep draw!
		if((zobristHigher == hashVal.hashHighBits) && (zobristLower == hashVal.hashLower)) { //TODO: assign bestMove - this can blow up in our face easily TODO: add proper checkmate handling
			int16_t oldEval  = hashVal.eval;
			if((depth <= hashVal.depth) && (oldEval > -10000) && (oldEval < 10000) && (oldEval != 0)){ //TODO: the != 0 is stupid, but somehwere something goes wrong with 3fold rep scores, so excluded ehre for safety
				if( ((hashVal.flag == FAILHIGH) || (hashVal.flag == FULLSEARCH)) && (oldEval >= beta)){
					setSearchId(searchId, position->zobristHash, hashVal.index);
					return beta;
				}
				else if( ((hashVal.flag == FAILLOW) || (hashVal.flag == FULLSEARCH)) && (oldEval <= alpha)){
					setSearchId(searchId, position->zobristHash, hashVal.index);
					return alpha; //node will fail low
				}
				else if((hashVal.flag == FULLSEARCH)){ //TODO: this condition can be vastly improved
					setSearchId(searchId, position->zobristHash, hashVal.index);
					if(oldEval <= alpha){
						return alpha;
					}
					if(oldEval >= beta){
						return beta;
					}
					return oldEval;
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
	uint64_t ownKing = position->pieceTables[position->toMove][king];
	bool movingSideInCheck = isFieldAttacked(position, (playerColor) (1-position->toMove), findLSB(ownKing));

	uint16_t refutationTarget = NO_REFUTATION; //invalid
	if(allowNullMove && !movingSideInCheck && (depth >= 2)){
		if(check_nullmove(position, &refutationTarget, ply, max_ply, depth, beta)){
			return beta;
		}
	}
	//remembers the index of best move after sorting
	//-------------------------------------------
	int16_t bestIndex = -1;

	//futility pruning
	//-----------------
	if(depth == 1){
		if(!movingSideInCheck) {
			searchCounts.futility_tried++;
			int32_t base = evaluation(position, alpha-151, alpha);
			if(base+150 < alpha){
				searchCounts.futility_successful++;
				//in this case, trying a silent move is pointless.
				//std::cout << "Successful futility pruning" << std::endl;
				return  negamaxQuiescence(position, alpha, beta, 0);
			}
		}
	}

	uint16_t stackCounter = qmvStack.getCounter();
	vdt_vector<chessMove> moves = qmvStack.getNext();
	generateAllMoves(&moves, position);
	bool isInCheck = orderStandardMoves(position, &moves, ply, hashVal.bestMove, refutationTarget);

	assert(isInCheck == movingSideInCheck);
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
			if(ind > 0){
				assert(moves[ind].type != kingMove); //all king moves moving into check should be found by move ordering!
			}
		} else {



			searchCounts.nodes[depth]++;
			searchCounts.totalNodes++;
			legalMovesAvailable = true;
			chessMove mv;
			uint16_t ownkingField = findLSB( position->pieceTables[position->toMove][king]);
			bool check = isFieldAttacked(position,  (playerColor) (1-position->toMove), ownkingField); //the last move gave check!
			uint16_t reduction = 0;
			uint16_t extension = 0;
			if(!check && !isInCheck && (moves[ind].captureType == none) && (depth > 2)){
				if((ind > 3) || foundGoodMove){
					reduction = 1;
					if(ind > 15) {
						reduction = 2;
					}
				}
			}

			/*if(reduction != fake_reduction){
				std::cout << "Different reductions found " << reduction << " " << fake_reduction <<std::endl;
			}*/

			/*if((ind > 3) &&  && (depth > 2) && !isInCheck) { //LMR
				if(!check){

				}
			}*/

			if(check && ((ply+depth < max_ply-1) || ((depth == 1) && (ply+depth < max_ply)) )){
				extension = 1;
				reduction = 0;
			}

			//PVSearch, currently a small gain for us with the > 3
			//-------------------------------------------------
			if(((ind > 3) || foundGoodMove )&& (depth > 2)) {
				int32_t value = -negamax(position, ply+1, max_ply, depth-1-reduction+extension, -alpha-1, -alpha, &mv);
				if(value < alpha+1){
					undoMove(position);
					continue;
				}
			}

			int32_t value = -negamax(position, ply+1, max_ply, depth-1+extension, -beta, -alpha, &mv);

			if(value > alpha){
				foundGoodMove = true;
				alpha = value;
				*bestMove = moves[ind];
				bestIndex = ind;
			}
		}
		undoMove(position);

		if((alpha >= beta)) {
			setHashEntry(FAILHIGH, beta, depth, searchId, (bestMove->sourceField | (bestMove->targetField << 8)), position->zobristHash);
			if(bestIndex != -1){
				searchCounts.bestIndex[depth][bestIndex]++;
			}
			if(bestMove->captureType == none){
				uint16_t toRemember = (bestMove->sourceField | (bestMove->targetField << 8));

				if ( (killerMoves[ply][0] != toRemember)) {
					killerMoves[ply][1] = killerMoves[ply][0];
					killerMoves[ply][0] = toRemember;
				}
			}

			qmvStack.release();
			assert(stackCounter == qmvStack.getCounter());
			return beta;
		}
	}

	if(bestIndex != -1){
		if(bestMove->captureType == none){
			historyTable[position->toMove][bestMove->sourceField][bestMove->targetField] += 2*depth*depth;
			if(historyTable[position->toMove][bestMove->sourceField][bestMove->targetField] > HISTORY_CUTOFF){
				//std::cout << "Rescaling history table " << std::endl;
				rescaleHistoryTable();
			}
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
			alpha = -30000+ply;
		} else {
			alpha = 0;
		}
	}


	if(foundGoodMove) {
		setHashEntry(FULLSEARCH, alpha, depth, searchId, (bestMove->sourceField | (bestMove->targetField << 8)), position->zobristHash);
	} else { //we failed low, remember as well
		setHashEntry(FAILLOW, alpha, depth, searchId, 0, position->zobristHash);
	}

	qmvStack.release();
	assert(stackCounter == qmvStack.getCounter());
	return alpha;
}
