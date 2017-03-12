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
uint64_t perftNodes(chessPosition* position, uint16_t depth){
	if(depth == 0){
		return 1;
	}
	uint64_t nodes = 0;
	vdt_vector<chessMove> moves = vdt_vector<chessMove>(150);
	generateAllMoves(&moves, position);
	calculateStandardSortEvals(position, &moves, 0, 0, NO_REFUTATION);
	std::sort(moves.data, moves.data+moves.length);
	bool isMate = true;
	for(uint16_t ind=0; ind < moves.length; ind++){
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
			isMate = false;
			additional_nodes = perftNodes(position, depth-1);

			nodes = nodes+additional_nodes;
		}

		undoMove(position);

		if(!isMate){
			if(depth == 6) {
				//std::cout << moveToString(moves[ind], *c) << " : " << additional_nodes << std::endl;
			}
		}
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
