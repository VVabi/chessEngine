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
searchDebugData searchCounts;

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


enum sortState {not_sorted, hash_handled, good_captures_handled, killers_handled, fully_sorted};
extern int16_t figureValues[7];

static inline bool getGoodCaptureToFront(vdt_vector<chessMove>* moves, uint16_t start_index) {

	int16_t best_index = -1;

	int16_t best = 0;

	for(uint16_t ind=start_index; ind < moves->length; ind++) {
		chessMove mv = (*moves)[ind];
		if(mv.captureType == none) {
			continue;
		}

		if( ((uint16_t) mv.captureType) >= ((uint16_t) mv.type)) { // || ((mv.captureType == knight) && (mv.type == bishopMove))){
			int16_t local_best = 5*figureValues[mv.captureType]/2-figureValues[mv.type];
			if(local_best > best) {
				best = local_best;
				best_index = ind;
			}
		}
	}

	if(best_index > -1) {
		chessMove buffer = (*moves)[start_index];
		(*moves)[start_index] = (*moves)[best_index];
		(*moves)[best_index] = buffer;
		return true;
	}

	return false;

}


static inline bool getHashMoveToFront(vdt_vector<chessMove>* moves, uint16_t hashMove, uint16_t startIndex) {

	for(uint16_t ind=startIndex; ind < moves->length; ind++) {
		chessMove mv = (*moves)[ind];
		if( (((mv.sourceField) | (mv.targetField << 8)) == hashMove) && (mv.type != castlingKingside) && (mv.type != castlingQueenside)){
			 chessMove buffer = (*moves)[startIndex];
			 (*moves)[startIndex] = (*moves)[ind];
			 (*moves)[ind] = buffer;
			 return true;
		}

	}
	return false;
}

uint16_t killerMoves[50][2];

//static chessMove buffer[5000];

static moveStack qmvStack;

uint16_t repetitionData[16384] = {0};

//std::ofstream logger("/home/vabi/log8.txt");

static inline void get_extensions_reductions(chessPosition* position, uint16_t* reduction, uint16_t* extension, bool check, bool movingSideInCheck, uint16_t ply, uint16_t max_ply, int16_t depth, chessMove* move, uint16_t ind) {
//#ifdef EXPERIMENTAL
		if(!check && !movingSideInCheck && (move->captureType == none) && (depth > 2) && (ply > 0)){
			if(move->sortEval < 50){
				*reduction = 1;
				if((move->sortEval < -50)) {
					*reduction = 2;
					if(depth > 10) {
						*reduction = 3;
					}
				}
			}
		}

		if(ind > 1000) {
			*reduction = 0;
		}
/*#else
		if(!check && !movingSideInCheck && (move->captureType == none) && (depth > 2) && (ply > 0)){
				if(ind > 3){
					*reduction = 1;
					if((move->sortEval < -50)) {
						*reduction = 2;
					}
				}
			}
#endif*/

/*#ifdef EXPERIMENTAL
		if(check) {
			*reduction = 0; //TODO: A check should NEVER be reduced, indepeoent of the ply/depth stuff
			if(((ply+depth < max_ply-1) || ((depth == 1) && (ply+depth < max_ply)) )){
				if(SEE(position, move) > -50) {
					*extension = 1;
				}
			}

		}

		bool closeToPromotion = (move->type == pawnMove) && ( (move->targetField > 48) || (move->targetField < 16));

		if( (closeToPromotion || (move->type == promotionQueen))) {

			*reduction = 0;

			if((ply+depth+*extension <= max_ply-1)) {
				*extension = *extension+1;
			}

		}

#else*/

		if(check && ((ply+depth < max_ply-1) || ((depth == 1) && (ply+depth < max_ply)) )){
			*reduction = 0; //TODO: A check should NEVER be reduced, independent of the ply/depth stuff
			if(SEE(position, move) > -50) {
				*extension = 1;
			}
		}

		bool closeToPromotion = (move->type == pawnMove) && ( (move->targetField > 48) || (move->targetField < 16));

		if( (closeToPromotion || (move->type == promotionQueen)) && (ply+depth+*extension < max_ply-1)) {
			*extension = *extension+1;
			*reduction = 0;
		}

		/*bool toSixthRank = false;

		if(move->type == pawnMove) {
			if((position->toMove == white) && (move->targetField > 40)) {
				toSixthRank = true;
			}
			if((position->toMove == black) && (move->targetField < 24)) {
				toSixthRank = true;
			}

		}

		if(toSixthRank ||  (move->type == promotionQueen)) {
			*reduction = 0;
			if((ply+depth+*extension < max_ply-1)) {
				*extension = *extension+1;
			}
		}*/
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

static uint8_t nullmoveReductions[40] = {0,1,2,2,2,3,3,3,
										 3,3,3,3,3,3,3,3,
										 3,3,3,3,3,3,3,3,
										 3,3,3,3,3,3,3,3,
										 3,3,3,3,3,3,3,3,
};

//std::ofstream badMoveLogger("/home/vabi/Tools/lateCutoffs.txt");
/*#ifdef EXPERIMENTAL
#define PREMARGIN 200
#define MARGIN 300
#else*/
//#endif

static inline bool check_futility(bool movingSideInCheck, int32_t alpha, chessPosition* position, int16_t premargin, int16_t margin) {

	if(!movingSideInCheck && (alpha > -2000)) {
		searchCounts.futility_tried++;
		int32_t simpleEval = evaluation(position, alpha-premargin-1, alpha, true);
		if(simpleEval < alpha-premargin) {
			int32_t base = evaluation(position, alpha-margin-1, alpha);
			if(base+margin < alpha){
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

//#ifdef EXPERIMENTAL
	bool noPieces = ((position->pieceTables[position->toMove][knight] || position->pieceTables[position->toMove][bishop] ||position->pieceTables[position->toMove][rook] ||position->pieceTables[position->toMove][queen]) == 0);
	bool opponentNoPawns = (position->pieceTables[1-position->toMove][pawn] == 0);


	if(noPieces || opponentNoPawns) {
		return false;
	}
//#endif

	int32_t eval = evaluation(position, beta-1, beta);
	if(eval < beta){
		return false; //no point in trying nullmove when the current evaluation is already worse than beta
	}

	makeNullMove(position);
	searchCounts.nullMovePruningTried++;
	pvLine dummy;
	dummy.numMoves = 0;
	dummy.line[0].targetField = NO_REFUTATION;
	int32_t value = -negamax(position, ply+1, max_ply-nullmoveReductions[depth], 0, depth-1-nullmoveReductions[depth], -beta, -beta+1, &dummy, false);
	undoNullMove(position);
	if(value < beta) {
		*refutationMoveTarget = dummy.line[0].targetField;
		assert(*refutationMoveTarget < 65);
		return false; //didnt manage a cutoff :(
	}
	searchCounts.nullMovePruningSuccessful++;
	return true;
}

static inline bool checkHashTable(int16_t* eval, uint16_t* hashMove, bool doHashProbe, uint64_t zobristHash, int16_t* alpha, int16_t* beta, int16_t depth) {
	hashEntry hashVal      = getHashTableEntry(zobristHash);
	uint32_t zobristHigher = (uint32_t) (zobristHash >> 32);
	uint16_t zobristLower  = (uint16_t) (((uint32_t) (zobristHash & 0xFFFFFFFF)) >> 16);
	bool isHit = (zobristHigher == hashVal.hashHighBits) && (zobristLower == hashVal.hashLower);

	if(isHit) {
		*hashMove = hashVal.bestMove;
	}

	assert( !((*hashMove != 0) && !isHit));
	if(doHashProbe){ //TODO: we should check whether another move leads to 3fold rep draw!
		if(isHit) { //TODO: assign bestMove - this can blow up in our face easily TODO: add proper checkmate handling
			int16_t oldEval  = hashVal.eval;
			if((depth <= hashVal.depth) && (oldEval > -10000) && (oldEval < 10000) && (oldEval != 0)){ //TODO: the != 0 is stupid, but somehwere something goes wrong with 3fold rep scores, so excluded ehre for safety
				if( ((hashVal.flag == FAILHIGH) || (hashVal.flag == FULLSEARCH)) && (oldEval >= *beta)){
					setSearchId(searchId, zobristHash, hashVal.index);
					*eval = *beta;
					return true;
				}
				else if( ((hashVal.flag == FAILLOW) || (hashVal.flag == FULLSEARCH)) && (oldEval <= *alpha)){
					setSearchId(searchId, zobristHash, hashVal.index);
					*eval =  *alpha; //node will fail low
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
		historyTable[toMove][bestMove->sourceField][bestMove->targetField] += depth*depth;
		int32_t cutoff = HISTORY_CUTOFF;
		if(historyTable[toMove][bestMove->sourceField][bestMove->targetField] > cutoff){
			rescaleHistoryTable();
		}
	}
	for(uint16_t cnt=0; cnt < bestIndex; cnt++){
		chessMove mv = (*moves)[cnt];
		if(mv.captureType == none){
			int32_t cutoff = HISTORY_CUTOFF;
			historyTable[toMove][mv.sourceField][mv.targetField] -= (depth*depth);
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
		//std::cout << "Total time " << totalTime << std::endl;
		throw timeoutException();
	}
	if(!continueSearch) {
		qmvStack.reset();
		throw timeoutException();
	}
}

//std::ofstream plogger("/home/vabi/Tools/log.txt");

#define DEFAULT_SORTEVAL 10000

uint64_t gotHashMove = 0;
uint64_t noHashMove   = 0;
int16_t negamax(chessPosition* position, uint16_t ply, uint16_t max_ply, uint16_t qply, int16_t depth, int16_t alpha, int16_t beta, pvLine* PV, bool allowNullMove, bool doHashProbe, bool extendChecks) {

	//check for timeout/interruption
	//------------------------------
	uint16_t numMoves = PV->numMoves;
	if(depth > 3) { //TODO: be careful here - we may have to reset the stack in quiescence search as well!
		PV->numMoves = 0;
		checkTimeout();
	}
	PV->numMoves = numMoves;
	assert(alpha < beta);
	assert(ply+depth <= max_ply);

	searchCounts.called++;

	//50 move rule
	//-----------------------------
	if((ply > 0) && (position->data.fiftyMoveRuleCounter >= 100)){
		PV->numMoves = 0;
		return 0;
	}

	//repetition check. don't check repetition in root to avoid stupid losses. TODO: this is a quickfix and may have unintended consequences!
	//------------------------------------------------------------------------------------------
	if ((ply > 0) && (repetitionData[position->zobristHash & 16383] > 1)) {
		if (backtrack_position_for_repetition(position)) {
			searchCounts.threefold_repetitions++;
			PV->numMoves = 0;
			return 0;
		} else {
			searchCounts.fake_3fold_repetitions++;
		}
	}

	//go to quiescence on depth 0.
	//---------------------------
	if(depth <= 0) {
		searchCounts.wentToQuiescence++;
		PV->numMoves = 0;
		return negamaxQuiescence(position, qply, ply, alpha, beta, 0);
	}

	//check in hashtable for position value
	//----------------------------------------
	uint16_t hashmove = 0;
	int16_t hashEval = 0;
	if(checkHashTable(&hashEval, &hashmove, doHashProbe, position->zobristHash, &alpha, &beta, depth)) {
		PV->numMoves = 0;
		return hashEval;
	}


	//try nullmove pruning
	//-------------------------
	uint16_t refutationTarget = NO_REFUTATION; //invalid
	uint64_t ownKing = position->pieceTables[position->toMove][king];
	bool movingSideInCheck = isFieldAttacked(position, (playerColor) (1-position->toMove), findLSB(ownKing));
	if(allowNullMove && !movingSideInCheck && (depth >= 2)){
		if(check_nullmove(position, &refutationTarget, ply, max_ply, depth, beta)){
			PV->numMoves = 0;
			return beta;
		}
	}

	//futility pruning
	//-----------------
	if(depth == 1) {
		if(check_futility(movingSideInCheck, alpha, position, 100,150)) {
			PV->numMoves = 0;
			return  negamaxQuiescence(position, qply, ply, alpha, beta, 0);
		}
	}

	if(depth == 2) {
		if(check_futility(movingSideInCheck, alpha, position, 500, 600)) {
			PV->numMoves = 0;
			return  negamaxQuiescence(position, 0, ply, alpha, beta, 0);
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

	if(hashmove == 0) {
		noHashMove++;
	} else {
		gotHashMove++;
	}

	//calc sorteval
	//------------------------

	//init variables
	//-----------------------------
	uint16_t numlegalMoves = 0;
	bool foundGoodMove = false;
	searchCounts.wentToSearch++;

	//remembers the index of best move after sorting
	//-------------------------------------------
	int16_t bestIndex = -1;

	pvLine localPV;
	localPV.numMoves = 0;


	sortState currentState = not_sorted;

	if(movingSideInCheck) {
		currentState = killers_handled;
	}

	for(uint16_t ind=0; ind < moves.length; ind++){
		switch(currentState) {
		//TODO: get rid of the fallthroughs
			case not_sorted:
				assert(ind == 0);
				currentState = hash_handled;
				if(getHashMoveToFront(&moves, hashmove, ind)) {
					moves[ind].sortEval = DEFAULT_SORTEVAL;
					break;
				}
			case hash_handled:
				if(getGoodCaptureToFront(&moves, ind)) {
					moves[ind].sortEval = DEFAULT_SORTEVAL;
					break;
				}
				currentState = good_captures_handled;
			case good_captures_handled: {
				uint16_t killerA = killerMoves[ply][0];
				if(getHashMoveToFront(&moves, killerA, ind)) {
					moves[ind].sortEval = DEFAULT_SORTEVAL;
					break;
				}
				uint16_t killerB = killerMoves[ply][1];
				if(getHashMoveToFront(&moves, killerB, ind)) {
					moves[ind].sortEval = DEFAULT_SORTEVAL;
					break;
				}
				currentState = killers_handled;
			}
			case killers_handled:
				//first move didn't produce cutoff, now we need to sort
				//------------------------------------------------------
				calculateStandardSortEvals(position, &moves, ind, ply, hashmove, refutationTarget);
				searchCounts.neededSort++;
				std::stable_sort(moves.data+ind, moves.data+moves.length);//stable sort makes the engine 100% predictable and comparable between different optimization levels
				currentState = fully_sorted;
				break;
			case fully_sorted:
				break;

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

		if(movingSideInCheck || (BIT64(moves[ind].sourceField) & (rookFieldTable[kingField] | bishopFieldTable[kingField])) || (moves[ind].type == kingMove)) {
			if(isFieldAttacked( position,  position->toMove, kingField)){
				/*if(moves[ind].type == kingMove) {
					std::cout << chessPositionToFenString(*position) << std::endl;
				}*/
				//move exposed our king, undo and continue
				//---------------------------------------------
                //assert(moves[ind].type != kingMove); //all king moves moving into check should be found by move ordering!
				undoMove(position);
				continue;
			}
		}
#ifdef DEBUG
		//debug check: if we overlooked an illegal move, we scream here
		//-------------------------------------------------------------
		if(isFieldAttacked( position,  position->toMove, kingField)) {
			logError("Illegal move detected");
		}
#endif
		searchCounts.nodes[depth]++;
		searchCounts.totalNodes++;
	    numlegalMoves++;

		uint16_t ownkingField = findLSB( position->pieceTables[position->toMove][king]);

		//check whether last move gave check (so its a check-check)
		//----------------------------------------------------------------
		bool check = isFieldAttacked(position,  (playerColor) (1-position->toMove), ownkingField);

		//get extensions and reductions
		//------------------------------
		uint16_t reduction = 0;
		uint16_t extension = 0;
//#ifdef EXPERIMENTAL

		get_extensions_reductions(position, &reduction, &extension, check, movingSideInCheck, ply, max_ply, depth, &moves[ind], ind);

//#endif
		if(!extendChecks){
			extension = 0;
		}

		//PVSearch, currently a small gain for us with the > 3
		//-------------------------------------------------
		if(((ind > 3) || foundGoodMove )&& (depth > 2)) {
			int32_t value = -negamax(position, ply+1, max_ply, qply, depth-1-reduction+extension, -alpha-1, -alpha, &localPV);
			if(value < alpha+1){
				undoMove(position);
				continue;
			}
		}

		//this is the real, full-fledged search now
		//-------------------------------------------
		int32_t value = -negamax(position, ply+1, max_ply, qply, depth-1+extension, -beta, -alpha, &localPV);

		//in case move is better than previous, remember
		//------------------------------------------------
		if(value > alpha){
			foundGoodMove = true;
			alpha = value;
			PV->line[0] = moves[ind];
			memcpy(PV->line+1, localPV.line, localPV.numMoves*sizeof(chessMove));
			PV->numMoves =  localPV.numMoves+1;
			bestIndex = ind;
		}

		undoMove(position);


		//in case of beta cutoff, leave
		//----------------------------------
		if((alpha >= beta)) {
			//if(ind > 0) {
				//badMoveLogger << moveToExtendedString(PV->line[0]) << " index " << ind << " of " << moves.length << " " << chessPositionToFenString(*position) << std::endl;
			//}

			handleBetaCutoff(&PV->line[0], position->zobristHash, beta, depth, ply, searchId);
			if(bestIndex != -1){
				searchCounts.bestIndex[depth][bestIndex]++;
				if(PV->line[0].captureType == none) {
					updateHistoryTables(&PV->line[0], depth, &moves, bestIndex, position->toMove);
				}
			}
			qmvStack.release();
			assert(stackCounter == qmvStack.getCounter());
			PV->numMoves = 0;
			return beta;
		}
	}

	//no cutoff. Check whether we found a good move, mate/stalemate handling accordingly. Then Cleanup/bookkeeping, then return alpha
	//-------------------------------------------------
	if(bestIndex != -1){
		updateHistoryTables(&PV->line[0], depth, &moves, bestIndex, position->toMove);
		searchCounts.bestIndex[depth][bestIndex]++;
	}

	//mate scores originate here!
	//------------------------------
	if(numlegalMoves == 0){
		if(movingSideInCheck){
			alpha = -30000+ply; //position is checkmate
		} else {
			alpha = 0; //position is stalemate
		}
	}

	//set hash entry
	//----------------------
	if(foundGoodMove) {
		setHashEntry(FULLSEARCH, alpha, depth, searchId, (PV->line[0].sourceField | (PV->line[0].targetField << 8)), position->zobristHash);
		if(PV->line[0].captureType == none){
				uint16_t toRemember = (PV->line[0].sourceField | (PV->line[0].targetField << 8));
				if ( (killerMoves[ply][0] != toRemember)) {
					killerMoves[ply][1] = killerMoves[ply][0];
					killerMoves[ply][0] = toRemember;
				}
			}
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
