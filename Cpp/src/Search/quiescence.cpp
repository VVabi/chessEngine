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
#include <algorithm>
#include <assert.h>
#include <userInterface/UIlayer.hpp>
#include <parameters/parameters.hpp>
#include <fstream>
static int32_t qindices[150] = {0};
extern uint8_t searchId;
void resetqIndices(){
	memset(qindices, 0, 150*sizeof(int32_t));
}

int32_t* getqIndexCounts(){
	return qindices;
}

static uint32_t nodes = 0;

void resetQuiescenceNodes(){
	nodes = 0;
}

uint32_t getQuiescenceNodes(){
	return nodes;
}

static uint32_t qcalled = 0;
static uint32_t sortqCalled = 0;

uint32_t getqCalled(){
	return qcalled;
}

void resetqCalled(){
	qcalled = 0;
}

uint32_t getSortqCalled(){
	return sortqCalled;
}

void resetSortqCalled(){
	sortqCalled = 0;
}

static moveStack mvStack;

static int counter = 0;
//std::ofstream quietData("/home/vabi/quiet.txt");

int16_t negamaxQuiescence(chessPosition* position, uint16_t qply, uint16_t ply, AlphaBeta alphabeta, uint16_t depth) {

	alphabeta.sanityCheck();

	const evalParameters* evalPars 						= getEvalParameters();
	uint64_t ownKing = position->pieceTables[position->toMove][king];
	if(isFieldAttacked(position, (playerColor) (1-position->toMove), findLSB(ownKing))) {
		pvLine line;
		return negamax(position, plyInfo(ply, ply+1, qply,1),  alphabeta, &line, searchSettings(nullmove_disabled, hashprobe_disabled, checkextension_disabled));
	}
#ifdef HASH
	hashEntry hashVal      = getHashTableEntry(position->zobristHash);

	uint32_t zobristHigher = (uint32_t) (position->zobristHash >> 32);
	uint16_t zobristLower  = (uint16_t) (((uint32_t) (position->zobristHash & 0xFFFFFFFF)) >> 16);
	uint16_t hashMove = hashVal.bestMove;
	if((zobristHigher == hashVal.hashHighBits) && (zobristLower == hashVal.hashLower)) { //TODO: assign bestMove - this can blow up in our face easily TODO: add proper checkmate handling
		int16_t oldEval  = hashVal.eval;
		if( (oldEval > -10000) && (oldEval < 10000) && (oldEval != 0)){ //TODO: the != 0 is stupid, but somewhere something goes wrong with 3fold rep scores, so excluded ehre for safety
			if( ((hashVal.flag == FAILHIGH) || (hashVal.flag == FULLSEARCH)) && (oldEval >= alphabeta.beta)){
				setSearchId(searchId, position->zobristHash, hashVal.index);
				return alphabeta.beta;
			}
			else if( ((hashVal.flag == FAILLOW) || (hashVal.flag == FULLSEARCH)) && (oldEval <= alphabeta.alpha)){
				setSearchId(searchId, position->zobristHash, hashVal.index);
				return alphabeta.alpha; //node will fail low
			}
			else if((hashVal.flag == FULLSEARCH)){ //TODO: this condition can be vastly improved
				setSearchId(searchId, position->zobristHash, hashVal.index);
				if(oldEval <= alphabeta.alpha){
					return alphabeta.alpha;
				}
				if(oldEval >= alphabeta.beta){
					return alphabeta.beta;
				}
				return oldEval;
			}
		}
	}
#endif


	int32_t baseEval = evaluation(position, alphabeta.alpha, alphabeta.beta);

	if(baseEval > alphabeta.alpha){
		alphabeta.alpha = baseEval;
	}
	if(alphabeta.betacutoff()) {
		return alphabeta.beta;
	}
	//delta pruning preparations
	//--------------------------------
	int32_t marginDifference = alphabeta.alpha - baseEval; //always >= 0!
	assert(marginDifference >= 0);

	//position is truely hopeless.Note that this DOES NOT SAVE NODES - the individual checks below will also fail! But we save some processing time
	//------------------------------------
	if(marginDifference > 1100) { //TODO: take promotions into account!
		return alphabeta.alpha;
	}

	chessMove bestMove;
	bestMove.sourceField = 0;
	bestMove.targetField = 0;
	uint16_t stackCounter = mvStack.getCounter();
	vdt_vector<chessMove> moves = mvStack.getNext();
	generateAllCaptureMoves(&moves, position);

#ifdef DEBUG
	vdt_vector<chessMove> allMoves = mvStack.getNext();
	generateAllMoves(&allMoves, position);

	uint32_t capturecnt = 0;

	for(uint16_t ind=0; ind < allMoves.length; ind++) {
		if (allMoves[ind].captureType != none) {
			capturecnt++;
		}
	}

	if(capturecnt != moves.length) {
		logError("Capture move generation is wrong");
		/*std::cout << "Capture move generation is wrong" << std::endl;
		std::cout << chessPositionToFenString(*position) << std::endl;
		std::cout << "capture move length " << moves.length << std::endl;
		std::cout << "captures" << std::endl;
		for (uint16_t ind = 0; ind < moves.length; ind++) {
			std::cout << moveToString(moves[ind]) << std::endl;
		}
		std::cout << "captures in all moves " << capturecnt << std::endl;
		for(uint16_t ind=0; ind < allMoves.length; ind++) {
			if (allMoves[ind].captureType != none) {
				std::cout << moveToString(allMoves[ind]) << std::endl;
			}
		}*/
	}
	mvStack.release();

#endif

	counter++;
	if(qply < 1) {
		generateChecks(&moves,position);

		/*if(moves.length == 0) {
			if(counter > 1000) {
				quietData << chessPositionToFenString(*position) << std::endl;
				counter = 0;
			}
		}*/
	}




	orderCaptureMoves(position, &moves, hashMove);
	if(moves.length > 0){
		qcalled++;
	}



	int16_t bestIndex = -1;
	for(uint16_t ind=0; ind < moves.length; ind++){
		if(ind == 1){
			sortqCalled++;
			std::stable_sort(moves.data, moves.data+moves.length);
		}

		//This has to be done AFTER sorting of course - that was a nasty bug
		//-----------------------
		if(moves[ind].sortEval < -50){
			break; //SEE pruning
		}

		//delta pruning. TODO: make define
		//----------------------------------------------------
		int32_t gainValue = evalPars->figureValues[moves[ind].captureType];
		if(gainValue < marginDifference-200){
			continue;
		}


		makeMove(&moves[ind], position);
		uint16_t kingField = findLSB( position->pieceTables[1- position->toMove][king]);

		if(isFieldAttacked( position,  position->toMove, kingField)){

		} else {
			nodes++;
			int32_t value = -negamaxQuiescence(position, qply+1, ply+1, alphabeta.invert(), depth+1);
			if(alphabeta.update(value)){
				bestMove = moves[ind];
				bestIndex = ind;
			}
		}
		undoMove(position);
		if(alphabeta.betacutoff()) {
#ifdef HASH
			//setHashEntry(FAILHIGH, alpha, 0, searchId, (bestMove.sourceField | (bestMove.targetField << 8)), position->zobristHash);
#endif
			mvStack.release();
			assert(stackCounter == mvStack.getCounter());
			if(bestIndex != -1){
				qindices[bestIndex]++;
			}
			return alphabeta.beta;
		}
	}

#ifdef HASH
	/*if(bestIndex != -1){
		qindices[bestIndex]++;
		setHashEntry(FULLSEARCH, alpha, 0, searchId, (bestMove.sourceField | (bestMove.targetField << 8)), position->zobristHash);
	} else {
		setHashEntry(FAILLOW, alpha, 0, searchId, 0, position->zobristHash);
	}*/
#endif
	mvStack.release();
	assert(stackCounter == mvStack.getCounter());
	return alphabeta.alpha;
}


