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

static int32_t qindices[150] = {0};
extern int16_t figureValues[];

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


int16_t negamaxQuiescence(chessPosition* position, int16_t alpha, int16_t beta, uint16_t depth) {

	assert(alpha < beta);
//#ifdef EXPERIMENTAL
	uint64_t ownKing = position->pieceTables[position->toMove][king]; //TODO: need to fix move stack first!!

	if(isFieldAttacked(position, (playerColor) (1-position->toMove), findLSB(ownKing))) {
		chessMove mv;
		return negamax(position, 30,31, 1, alpha, beta, &mv, false, false);
	}
//#endif



	int32_t baseEval = evaluation(position, alpha, beta);

	if(baseEval > alpha){
		alpha = baseEval;
	}
	if(alpha >= beta) {
		return beta;
	}
	//delta pruning preparations
	//--------------------------------
	int32_t marginDifference = alpha - baseEval; //always >= 0!
	assert(marginDifference >= 0);

	//position is truely hopeless.Note that this DOES NOT SAVE NODES - the individual checks below will also fail! But we save some processing time
	//------------------------------------
	if(marginDifference > 1100) { //TODO: take promotions into account!
		return alpha;
	}

	chessMove bestMove;
	bestMove.sourceField = 0;
	bestMove.targetField = 0;
	uint16_t stackCounter = mvStack.getCounter();
	vdt_vector<chessMove> moves = mvStack.getNext();
	generateAllCaptureMoves(&moves, position);
	orderCaptureMoves(position, &moves);
	if(moves.length > 0){
		qcalled++;
	}



	int16_t bestIndex = -1;
	for(uint16_t ind=0; ind < moves.length; ind++){

		if(moves[ind].sortEval < -50){ //TODO: This means we prune bishopxKnight! This is not intended
			break; //SEE pruning
		}
		if(ind == 1){
			sortqCalled++;
			std::sort(moves.data, moves.data+moves.length);
		}

		//delta pruning. TODO: make define
		//----------------------------------------------------
		int32_t gainValue = figureValues[moves[ind].captureType];
		if(gainValue < marginDifference-200){
			continue;
		}


		makeMove(&moves[ind], position);
		uint16_t kingField = findLSB( position->pieceTables[1- position->toMove][king]);

		if(isFieldAttacked( position,  position->toMove, kingField)){

		} else {
			nodes++;
			int32_t value = -negamaxQuiescence(position, -beta, -alpha, depth+1);
			if(value > alpha){
				alpha = value;
				bestMove = moves[ind];
				bestIndex = ind;
				/*if(moves[ind].sortEval < -200){
					std::cout << "WTF?? SEE seems to be wrong!" << std::endl;
					std::cout << chessPositionToOutputString(*position) << std::endl;
					std::cout << chessPositionToFenString(*position, false) << std::endl;
					std::cout << moves[ind].sourceField << " " << moves[ind].targetField << std::endl;
				}*/
			}
		}
		undoMove(position);
		if(alpha >= beta) {
			setHashMove(bestMove.sourceField | (bestMove.targetField << 8), position->zobristHash);
			mvStack.release();
			assert(stackCounter == mvStack.getCounter());
			if(bestIndex != -1){
				qindices[bestIndex]++;
			}
			return beta;
		}
	}

	if(bestIndex != -1){
		qindices[bestIndex]++;
		setHashMove(bestMove.sourceField | (bestMove.targetField << 8), position->zobristHash);
	}

	mvStack.release();
	assert(stackCounter == mvStack.getCounter());
	return alpha;



}


