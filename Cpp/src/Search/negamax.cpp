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
#include <atomic>
extern int32_t historyTable[2][64][64];
extern uint8_t searchId;
extern std::atomic<bool> continueSearch;
static searchDebugData searchCounts;

extern uint64_t bishopFieldTable[];
extern uint64_t rookFieldTable[];

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




uint64_t trueRets = 0;
uint64_t falseRets = 0;

bool getHashMoveToFront(vdt_vector<chessMove>* moves, uint16_t hashMove, chessMove* mov) {

	for(uint16_t ind=0; ind < moves->length; ind++) {
		chessMove mv = (*moves)[ind];
		if( ((mv.sourceField) | (mv.targetField << 8)) == hashMove) {
			 chessMove buffer = (*moves)[0];
			 (*moves)[0] = (*moves)[ind];
			 (*moves)[ind] = buffer;
			 *mov = mv;
			 trueRets++;
			 return true;
		}

	}
	falseRets++;
	return false;

}

uint16_t killerMoves[40][2];

//static chessMove buffer[5000];

static moveStack qmvStack;

uint16_t repetitionData[16384] = {0};

//std::ofstream logger("/home/vabi/log8.txt");

static inline void get_extensions_reductions(uint16_t* reduction, uint16_t* extension, bool check, bool movingSideInCheck, uint16_t ply, uint16_t max_ply, int16_t depth, chessMove* move, uint16_t ind) {

		if(!check && !movingSideInCheck && (move->captureType == none) && (depth > 2) && (ply > 0)){
			if((ind > 3)){
				*reduction = 1;
				if(move->sortEval < -50) {
					*reduction = 2;
				}
			}
		}

		if(check && ((ply+depth < max_ply-1) || ((depth == 1) && (ply+depth < max_ply)) )){
			*extension = 1;
			*reduction = 0;
		}

		bool closeToPromotion = (move->type == pawnMove) && ( (move->targetField > 48) || (move->targetField < 16));

		if( (closeToPromotion || (move->type == promotionQueen)) && (ply+depth+*extension < max_ply-1)) {
			*extension = *extension+1;
			*reduction = 0;
		}

}

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


static inline bool check_futility(bool movingSideInCheck, int32_t alpha, chessPosition* position) {
	if(!movingSideInCheck && (alpha > -2000)) {
		searchCounts.futility_tried++;
		int32_t simpleEval = evaluation(position, alpha-151, alpha, true);
		if(simpleEval < alpha+100) {
			int32_t base = evaluation(position, alpha-151, alpha);
			if(base+150 < alpha){
				searchCounts.futility_successful++;
				//in this case, trying a silent move is pointless.
				//std::cout << "Successful futility pruning" << std::endl;
				return true;
			}
		}
	}
	return false;
}

static inline bool check_nullmove(chessPosition* position, uint16_t* refutationMoveTarget, uint16_t ply, uint16_t max_ply, int16_t depth, int32_t beta){
	if(beta > 10000){ //TODO: more dynamic condition here?
		return false;
	}
	int32_t eval = evaluation(position, beta-1, beta);
	if(eval < beta){
		return false; //no point in trying nullmove when the current evaluation is already worse than beta
	}

	makeNullMove(position);
	chessMove mv;
	mv.targetField = 64;
	searchCounts.nullMovePruningTried++;
	int32_t value = -negamax(position, ply+1, max_ply-nullmoveReductions[depth], depth-1-nullmoveReductions[depth], -beta, -beta+1, &mv, false);
	undoNullMove(position);
	if(value < beta) {
		*refutationMoveTarget = mv.targetField;
		assert(*refutationMoveTarget < 65);
		return false; //didnt manage a cutoff :(
	}
	searchCounts.nullMovePruningSuccessful++;
	return true;
}

static inline bool checkHashTable(int16_t* eval, uint16_t* hashMove, bool doHashProbe, uint64_t zobristHash, int16_t alpha, int16_t beta, int16_t depth) {
	hashEntry hashVal      = getHashTableEntry(zobristHash);
	uint32_t zobristHigher = (uint32_t) (zobristHash >> 32);
	uint16_t zobristLower  = (uint16_t) (((uint32_t) (zobristHash & 0xFFFFFFFF)) >> 16);
	*hashMove = hashVal.bestMove;
	if(doHashProbe){ //TODO: we should check whether another move leads to 3fold rep draw!
		if((zobristHigher == hashVal.hashHighBits) && (zobristLower == hashVal.hashLower)) { //TODO: assign bestMove - this can blow up in our face easily TODO: add proper checkmate handling
			int16_t oldEval  = hashVal.eval;
			if((depth <= hashVal.depth) && (oldEval > -10000) && (oldEval < 10000) && (oldEval != 0)){ //TODO: the != 0 is stupid, but somehwere something goes wrong with 3fold rep scores, so excluded ehre for safety
				if( ((hashVal.flag == FAILHIGH) || (hashVal.flag == FULLSEARCH)) && (oldEval >= beta)){
					setSearchId(searchId, zobristHash, hashVal.index);
					*eval = beta;
					return true;
				}
				else if( ((hashVal.flag == FAILLOW) || (hashVal.flag == FULLSEARCH)) && (oldEval <= alpha)){
					setSearchId(searchId, zobristHash, hashVal.index);
					*eval =  alpha; //node will fail low
					return true;
				}
				else if((hashVal.flag == FULLSEARCH)){ //TODO: this condition can be vastly improved
					setSearchId(searchId, zobristHash, hashVal.index);
					*eval = oldEval;
					return true;
				}
			}
		}
	}
	return false;
}


static inline void updateHistoryTables(chessMove* bestMove, int16_t depth, vdt_vector<chessMove>* moves, uint16_t bestIndex, playerColor toMove) {
	if(bestMove->captureType == none){
		historyTable[toMove][bestMove->sourceField][bestMove->targetField] += 2*depth*depth;
		int32_t cutoff = HISTORY_CUTOFF;
		if(historyTable[toMove][bestMove->sourceField][bestMove->targetField] > cutoff){
			rescaleHistoryTable();
		}
	}
	for(uint16_t cnt=0; cnt < bestIndex; cnt++){
		chessMove mv = (*moves)[cnt];
		if(mv.captureType == none){
			int32_t cutoff = HISTORY_CUTOFF;
			historyTable[toMove][mv.sourceField][mv.targetField] -= depth*depth;
			if(historyTable[toMove][mv.sourceField][mv.targetField] < (-cutoff)){
				rescaleHistoryTable();
			}
		}
	}
}

static inline void handleBetaCutoff(chessMove* bestMove, uint64_t zobristHash, int16_t beta, int16_t depth, uint16_t ply, uint8_t searchId) {
	setHashEntry(FAILHIGH, beta, depth, searchId, (bestMove->sourceField | (bestMove->targetField << 8)), zobristHash);
	if(bestMove->captureType == none){
		uint16_t toRemember = (bestMove->sourceField | (bestMove->targetField << 8));
		if ( (killerMoves[ply][0] != toRemember)) {
			killerMoves[ply][1] = killerMoves[ply][0];
			killerMoves[ply][0] = toRemember;
		}
	}
}


static inline void checkTimeout() {
	if(get_timestamp()-start_ts >= totalTime){ //TODO: how is this performance wise?
		qmvStack.reset();
		std::cout << "Total time " << totalTime << std::endl;
		throw timeoutException();
	}
	if(!continueSearch) {
		qmvStack.reset();
		throw timeoutException();
	}
}

//std::ofstream plogger("/home/vabi/Tools/log.txt");

int16_t negamax(chessPosition* position, uint16_t ply, uint16_t max_ply, int16_t depth, int16_t alpha, int16_t beta, chessMove* bestMove, bool allowNullMove, bool doHashProbe){

	/*std::string pos =chessPositionToFenString(*position);
	plogger << pos << std::endl;

	if(pos == "r3k2r/p1pp1pb1/Bn3np1/3pq3/1p2P3/2N2Q1P/PPPB1P1P/R3K2R w KQkq - 0 5") {
		std::cout << "gotcha" << std::endl;
	}*/
	//check for timeout/interruption
	//------------------------------
	if(depth > 3) { //TODO: be careful here - we may have to reset the stack in quiescence search as well!
		checkTimeout();
	}
	assert(alpha < beta);
	assert(ply+depth <= max_ply);

	searchCounts.called++;

	//50 move rule
	//-----------------------------
	if(position->data.fiftyMoveRuleCounter >= 100){
		return 0;
	}

	//repetition check. don't check repetition in root to avoid stupid losses. TODO: this is a quickfix and may have unintended consequences!
	//------------------------------------------------------------------------------------------
	if ((ply > 0) && (repetitionData[position->zobristHash & 16383] > 1)) {
		if (backtrack_position_for_repetition(position)) {
			searchCounts.threefold_repetitions++;
			return 0;
		} else {
			searchCounts.fake_3fold_repetitions++;
		}
	}

	//check in hashtable for position value
	//----------------------------------------
	uint16_t hashmove = 0;
	int16_t hashEval = 0;
	if(checkHashTable(&hashEval, &hashmove, doHashProbe, position->zobristHash, alpha, beta, depth)) {
		return hashEval;
	}

	//go to quiescence on depth 0
	//---------------------------
	if(depth <= 0) {
		searchCounts.wentToQuiescence++;
		return negamaxQuiescence(position, alpha, beta, 0);
	}

	//try nullmove pruning
	//-------------------------
	uint16_t refutationTarget = NO_REFUTATION; //invalid
	uint64_t ownKing = position->pieceTables[position->toMove][king];
	bool movingSideInCheck = isFieldAttacked(position, (playerColor) (1-position->toMove), findLSB(ownKing));
	if(allowNullMove && !movingSideInCheck && (depth >= 2)){
		if(check_nullmove(position, &refutationTarget, ply, max_ply, depth, beta)){
			return beta;
		}
	}

	//futility pruning
	//-----------------
	if(depth == 1) {
		if(check_futility(movingSideInCheck, alpha, position)) {
			return  negamaxQuiescence(position, alpha, beta, 0);
		}
	}

	//------------------------------------------------------------------
	//now we are out of tricks, we need to start the actual search.
	//------------------------------------------------------------------

	//generate moves
	//------------------
	uint16_t stackCounter = qmvStack.getCounter();
	vdt_vector<chessMove> moves = qmvStack.getNext();
	generateAllMoves(&moves, position);

	//calc sorteval
	//------------------------
	bool isInCheck = calculateStandardSortEvals(position, &moves, ply, hashmove, refutationTarget); //does no complete sort, but puts best move at the front
	assert(isInCheck == movingSideInCheck);

	//init variables
	//-----------------------------
	bool legalMovesAvailable = false;
	bool foundGoodMove = false;
	searchCounts.wentToSearch++;

	//remembers the index of best move after sorting
	//-------------------------------------------
	int16_t bestIndex = -1;
	for(uint16_t ind=0; ind < moves.length; ind++){


		if(ind == 1){
			//first move didn't produce cutoff, now we need to sort
			//------------------------------------------------------
			searchCounts.neededSort++;
			std::stable_sort(moves.data, moves.data+moves.length);
		}

		//illegal move. Since list is sorted or, in case ind=0, best move is first, we can leave here: all further moves are also illegal.
		//---------------------------------------------------------------------------------------------------------------------------------
		if(moves[ind].sortEval < -10000){
			break;
		}

		//now make move and check for legality
		//---------------------------------------
		makeMove(&moves[ind], position);
		uint16_t kingField = findLSB( position->pieceTables[1- position->toMove][king]);

		if(isInCheck || (BIT64(moves[ind].sourceField) & (rookFieldTable[kingField] | bishopFieldTable[kingField]))) {
			if(isFieldAttacked( position,  position->toMove, kingField)){
				//move exposed our king, undo and continue
				//---------------------------------------------
                assert(moves[ind].type != kingMove); //all king moves moving into check should be found by move ordering!
				undoMove(position);
				continue;
			}
		}

//#ifdef DEBUG
		if(isFieldAttacked( position,  position->toMove, kingField)) {
			std::cout << chessPositionToOutputString(*position);
			std::cout <<  (rookFieldTable[kingField] | bishopFieldTable[kingField]);
			std::cout << "WTF" << std::endl;
		}
//#endif
		searchCounts.nodes[depth]++;
		searchCounts.totalNodes++;
		legalMovesAvailable = true;
		chessMove mv;
		uint16_t ownkingField = findLSB( position->pieceTables[position->toMove][king]);

		//check whether last move gave check (so its a check-check)
		//----------------------------------------------------------------
		bool check = isFieldAttacked(position,  (playerColor) (1-position->toMove), ownkingField);

		//get extensions and reductions
		//------------------------------
		uint16_t reduction = 0;
		uint16_t extension = 0;
		get_extensions_reductions(&reduction, &extension, check, movingSideInCheck, ply, max_ply, depth, &moves[ind], ind);

		//PVSearch, currently a small gain for us with the > 3
		//-------------------------------------------------
		if(((ind > 3) || foundGoodMove )&& (depth > 2)) {
			int32_t value = -negamax(position, ply+1, max_ply, depth-1-reduction+extension, -alpha-1, -alpha, &mv);
			if(value < alpha+1){
				undoMove(position);
				continue;
			}
		}

		//this is the real, full-fledged search now
		//-------------------------------------------
		int32_t value = -negamax(position, ply+1, max_ply, depth-1+extension, -beta, -alpha, &mv);

		//in case move is better than previous, remember
		//------------------------------------------------
		if(value > alpha){
			foundGoodMove = true;
			alpha = value;
			*bestMove = moves[ind];
			bestIndex = ind;
		}

		undoMove(position);

		//in case of beta cutoff, leave
		//----------------------------------
		if((alpha >= beta)) {
			handleBetaCutoff(bestMove, position->zobristHash, beta, depth, ply, searchId);
			if(bestIndex != -1){
				searchCounts.bestIndex[depth][bestIndex]++;
			}
			qmvStack.release();
			assert(stackCounter == qmvStack.getCounter());
			return beta;
		}
	}

	//no cutoff. Check whether we found a good move, mate/stalemate handling accordingly. Then Cleanup/bookkeeping, then return alpha
	//-------------------------------------------------
	if(bestIndex != -1){
		updateHistoryTables(bestMove, depth, &moves, bestIndex, position->toMove);
		searchCounts.bestIndex[depth][bestIndex]++;
	}

	//mate scores originate here!
	//------------------------------
	if(!legalMovesAvailable){
		if(movingSideInCheck){
			alpha = -30000+ply;
		} else {
			alpha = 0;
		}
	}

	//set hash entry
	//----------------------
	if(foundGoodMove) {
		setHashEntry(FULLSEARCH, alpha, depth, searchId, (bestMove->sourceField | (bestMove->targetField << 8)), position->zobristHash);
	} else { //we failed low, remember as well
		setHashEntry(FAILLOW, alpha, depth, searchId, 0, position->zobristHash);
	}
	//return memory
	//---------------------
	qmvStack.release();
	assert(stackCounter == qmvStack.getCounter());

	//and finally return alpha
	//--------------------------
	return alpha;
}
