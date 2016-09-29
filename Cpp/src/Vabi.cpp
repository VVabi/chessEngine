//============================================================================
// Name        : Vabi.cpp
// Author      : Fabian Lenhardt
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;
#include <lib/basics.hpp>
#include <lib/moveMaking/moveMaking.hpp>
#include <tests/tests.hpp>
#include <DataTypes/vdt_vector.hpp>
#include <lib/moveGeneration/moveGeneration.hpp>
#include <fstream>
#include <DataTypes/threadSafeQueue.hpp>
#include <tr1/memory>
#include <lib/Attacks/attacks.hpp>
#include <lib/bitfiddling.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <Search/search.hpp>
#include <hashTables/hashTables.hpp>
#include <selfPlaying/selfPlaying.hpp>
#include <lib/Evaluation/evaluation.hpp>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <userInterface/userInterface.hpp>
#include <userInterface/networkUserInterface.hpp>

extern int32_t completePieceTables[7][2][64];
extern int16_t endGamepieceTables[7][2][64];
extern int16_t pieceTables[7][2][64];
extern uint16_t figureValues[];


uint64_t get_timestamp(){
	struct timeval start;
	uint64_t mtime, seconds, useconds;
	gettimeofday(&start, NULL);
	seconds  = start.tv_sec;
	useconds = start.tv_usec;

	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
	return mtime;

}

void outPutuint64(uint64_t num){

	for	(int j=7; j >= 0; j--)	{
		for(int i=0; i < 8; i++){
			uint64_t toCheck = (1UL << (8*j+i));
			if(num & toCheck) {
				std::cout << "1";
			}
			else{
				std::cout << "0";
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;


}




uint32_t runSinglePositionPerformanceTest(std::string position, uint16_t depth) {
	chessPosition c = stringToChessPosition(position);
	c.castlingRights = 0;
	c.toMove = (position.at(66) == 'w' ? white: black);
	resetNodes();
	chessMove bestMove;
	resetQuiescenceNodes();
	struct timeval start, end;
	//long mtime, seconds, useconds;
	gettimeofday(&start, NULL);
	//int32_t eval = negamax(&c, depth, -100000, 100000, &bestMove);
	negamax(&c, depth, -100000, 100000, &bestMove);
	gettimeofday(&end, NULL);

	/*seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;

	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;*/
	uint32_t nodeCount = getNodes()+getQuiescenceNodes();
	//double nps = ((double) nodeCount)/((double) mtime)*1000.0;
	/*std::cout << "Searched " <<  nodeCount << " positions in " << mtime << " for " << nps << " nodes per second" << std::endl;
	std::cout << "Forced move " << moveToString(bestMove, c) << std::endl;
	std::cout << "Evaluation "  << eval << std::endl;*/

	return nodeCount;

}
networkUserInterface* userInterface;

uint32_t searchMove(chessPosition* position, chessMove* bestMove, uint32_t maximal_time, uint32_t* nodeCount, uint64_t* mtime, int32_t* eval) {
	resetNodes();
	resetQuiescenceNodes();
	resetIndices();
	uint64_t start_ts  = get_timestamp();
	uint16_t depth = 6;
	*eval = 0;
	while((get_timestamp()-start_ts < maximal_time) && (depth < 14)) {
		*eval = negamax(position, depth, -100005, 100005, bestMove);

		std::cout << depth <<std::endl;
		*nodeCount = getNodes()+getQuiescenceNodes();
		*mtime = get_timestamp()-start_ts;
		userInterface->sendSearchInfo(*nodeCount, *mtime, *eval, depth, moveToString(*bestMove, *position));
		depth++;
		if(*eval > 90000) {
			break;
		}
	}
	depth--;
	std::cout << "Depth searched " << depth << std::endl;
	*mtime = get_timestamp()-start_ts;

	double nps = ((double) *nodeCount)/((double) *mtime)*1000.0;
	std::cout << "Searched " <<  *nodeCount << " positions in " << *mtime << " for " << nps << " nodes per second" << std::endl;
	std::cout << "Forced move " << moveToString(*bestMove, *position) << std::endl;
	std::cout << "Evaluation "  << eval << std::endl;
	int32_t* indices = getIndexCounts();
	for(uint16_t k=0; k < 50; k++){
		std::cout << indices[k] << " ";
	}
	std::cout << std::endl;
	return depth;
}




int main() {

	for(uint32_t index=0; index < 7; index++) {
		for(uint32_t t=0; t < 2; t++) {
			for(uint32_t k=0; k < 64; k++) {
					completePieceTables[index][t][k] = ((uint16_t) (pieceTables[index][t][k]+figureValues[index])) |  ( ((uint16_t) (endGamepieceTables[index][t][k]+figureValues[index])) << 16);
			}
		}
	}

	srand (time(NULL));
	fillZobristHash();
	//runTests();
	/*std::string positionStrTest = "R000K00RP0PBBPPP00P00Q0p0000P000000P0000bn00pnp0p0pNqpb0r000k00rbKQkq";
	chessPosition positionT = stringToChessPosition(positionStrTest);
	vdt_vector<chessMove> moves = vdt_vector<chessMove>(100);

	generateAllMoves(&moves, &positionT);
	orderStandardMoves(&positionT, &moves);*/
	//return 0;
	userInterface = new networkUserInterface();


	std::string positionstr = "RNBQKBNRPPPPPPPP00000000000000000000000000000000pppppppprnbqkbnrwKQkq";
	chessPosition position = stringToChessPosition(positionstr);
	std::string move;

	while(1){
		if(userInterface->receiveNewPosition(positionstr)){
			free_position(&position);
			position = stringToChessPosition(positionstr);
			userInterface->sendNewPosition(positionstr);
		}
		if(userInterface->receiveMove(move)){
			vdt_vector<chessMove> moves = vdt_vector<chessMove>(100);
			uint64_t mv = stringToMove(move);
			generateAllMoves(&moves, &position);
			orderStandardMoves(&position, &moves);
			bool found = false;
			chessMove m;
			for(uint16_t ind=0; ind < moves.length; ind++) {
				if(moves[ind].sortEval < -10000) {
					break;
				}
				if(moves[ind].move == mv){
					found = true;
					m = moves[ind];
				}
			}
			if(found){
				std::cout << moveToString(m, position) << std::endl;
				makeMove(&m, &position);
				uint16_t kingField = findLSB(position.pieceTables[1-position.toMove][king]);
				if(isFieldAttacked(&position, position.toMove, kingField)){
					std::cout << "Illegal move!" << std::endl;
					undoMove(&position);
				} else {
					std::string newPosition = chessPositionToString(position);
					userInterface->sendNewPosition(newPosition);
					chessMove bestMove;
					uint32_t nodeCount;
					uint64_t mtime;
					int32_t eval = 0;
					searchMove(&position, &bestMove, 2000, &nodeCount, &mtime, &eval);
					std::cout << eval << std::endl;
					makeMove(&bestMove, &position);
					std::string newPosition2 = chessPositionToString(position);
					userInterface->sendNewPosition(newPosition2);

				}
			} else {
				std::cout << "Invalid move!" << std::endl;
			}
			moves.free_array();
		}

		if (userInterface->receiveUndoMove()) {
			std::cout << "Got undo event!" << std::endl;
			undoMove(&position);
			std::string newPosition = chessPositionToString(position);
			userInterface->sendNewPosition(newPosition);
		}

		if(userInterface->receiveForceMove()) {
			chessMove bestMove;
			uint32_t nodeCount;
			uint64_t mtime;
			int32_t eval = 0;
			searchMove(&position, &bestMove, 2000, &nodeCount, &mtime, &eval);
			makeMove(&bestMove, &position);
			std::string newPosition = chessPositionToString(position);
			userInterface->sendNewPosition(newPosition);

		}
	}




}
