/*
 * perftTests.cpp
 *
 *  Created on: Sep 24, 2016
 *      Author: vabi
 */

#include <lib/basics.hpp>
#include <lib/moveMaking/moveMaking.hpp>
#include <iostream>
#include <tests/tests.hpp>
#include <DataTypes/vdt_vector.hpp>
#include <lib/moveGeneration/moveGeneration.hpp>
#include <DataTypes/vdt_vector.hpp>
#include <lib/Attacks/attacks.hpp>
#include <lib/bitfiddling.h>
#include <Search/search.hpp>
#include <userInterface/UIlayer.hpp>
#include <algorithm>


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

uint64_t perftNodes(chessPosition* position, uint16_t depth){
	if(depth == 0){
		return 1;
	}
	uint64_t nodes = 0;
	vdt_vector<chessMove> moves = vdt_vector<chessMove>(150);
	generateAllMoves(&moves, position);
	/*calculateStandardSortEvals(position, &moves, 0, 0, 0, NO_REFUTATION);
	std::sort(moves.data, moves.data+moves.length);*/

	sortState currentState = not_sorted;
	uint64_t ownKing = position->pieceTables[position->toMove][king];
	bool movingSideInCheck = isFieldAttacked(position, (playerColor) (1-position->toMove), findLSB(ownKing));
	if(movingSideInCheck) {
		currentState = killers_handled;
	}



	for(uint16_t ind=0; ind < moves.length; ind++){

		switch(currentState) {
					case not_sorted:
						assert(ind == 0);
						currentState = hash_handled;
					case hash_handled:
						if(getGoodCaptureToFront(&moves, ind)) {
							moves[ind].sortEval = 10000;
							break;
						}
						currentState = good_captures_handled;
					case good_captures_handled: {
						currentState = killers_handled;
					}
					case killers_handled:
						//first move didn't produce cutoff, now we need to sort
						//------------------------------------------------------
						calculateStandardSortEvals(position, &moves, ind, 0, sortInfo(false, NO_REFUTATION, 0));

						std::stable_sort(moves.data+ind, moves.data+moves.length);//stable sort makes the engine 100% predictable and comparable between different optimization levels
						currentState = fully_sorted;
						break;
					case fully_sorted:
						break;

				}




		if(moves[ind].sortEval < -10000){
			break;
		}

		if(moves[ind].captureType != none){
			if(moves[ind].captureType == king){
				std::cout << "Taking the king should be impossible" << std::endl;
				uint16_t kingField = findLSB(position->pieceTables[1-position->toMove][king]);
				isFieldAttacked(position, position->toMove, kingField);
			}
		}

		makeMove(&moves[ind], position);
		uint16_t kingField = findLSB(position->pieceTables[1-position->toMove][king]);
		uint32_t additional_nodes = 0;

		if(isFieldAttacked(position, position->toMove, kingField)){

		} else {
			additional_nodes = perftNodes(position, depth-1);

			nodes = nodes+additional_nodes;
		}

		undoMove(position);
	}

	moves.free_array();
	return nodes;
}


//uint32_t perftReference = {8902, 197281,4865609,119060324,3195901860};

testResult testPerftTestSuite(){
	testResult ret;
	ret.passed   = true;
	ret.testName = "perftTest";

	std::string position = "RNBQKBNRPPPPPPPP00000000000000000000000000000000pppppppprnbqkbnrwKQkq";
	chessPosition c = stringToChessPosition(position);

	uint64_t nodes = perftNodes(&c,7);
	if(nodes != 3195901860) {
		std::cout <<"First position failed" <<std::endl;
		std::cout << nodes << std::endl;
		ret.passed = false;
		return ret;
	}
	std::cout << nodes << std::endl;
	position = "R000K00RPPPBBPPP00N00Q0p0p00P000000PN000bn00pnp0p0ppqpb0r000k00rwKQkq";
	c = stringToChessPosition(position);
	nodes = perftNodes(&c, 5);
	if(nodes != 193690690) {
		std::cout << "2nd position failed" << std::endl;
		std::cout << nodes << std::endl;
		ret.passed = false;
		return ret;
	}
	std::cout << nodes << std::endl;
	position = "000000000000P0P0000000000R000p0kKP00000r000p000000p0000000000000w0000";
	c = stringToChessPosition(position);
	nodes = perftNodes(&c,7);
	if(nodes != 178633661) {
		std::cout << "3rd position failed" <<std::endl;
		std::cout << nodes << std::endl;
		ret.passed = false;
		return ret;
	}
	std::cout << nodes << std::endl;



	return ret;

}
