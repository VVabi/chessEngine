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
#include <userInterface/uciInterface.hpp>
#include <algorithm>
#include <fstream>
#include <sstream>


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




uint64_t runSinglePositionPerformanceTest(std::string position, uint16_t depth, uint64_t* negamaxNodes, uint64_t* qNodes, bool useAspiration) {
	chessPosition c = stringToChessPosition(position);
	resetNodes();
	chessMove bestMove;
	resetQuiescenceNodes();
	struct timeval start, end;
	//long mtime, seconds, useconds;
	gettimeofday(&start, NULL);
	//int32_t eval = negamax(&c, depth, -100000, 100000, &bestMove);
	int32_t alpha = -100005;
	int32_t beta  = 100005;
	int32_t searchdepth = 3;
	while(searchdepth <= depth && (searchdepth < 14)) {

		bool succeeded = false;
		int32_t eval = negamax(&c, searchdepth, alpha, beta, &bestMove);

		if(useAspiration) {
			if ((eval <= alpha)) {
				alpha = alpha-100;
			} else if(eval >= beta){
				beta = beta+100;
			}

			else {
				succeeded = true;
			}


		} else {
			succeeded = true;
		}

		if(succeeded){
			searchdepth++;
			alpha = eval-50;
			beta  = eval+50;
		}
	}
	gettimeofday(&end, NULL);

	/*seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;

	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;*/
	uint64_t nodeCount = getNodes()+getQuiescenceNodes();
	*negamaxNodes = getNodes();
	*qNodes       = getQuiescenceNodes();
	//double nps = ((double) nodeCount)/((double) mtime)*1000.0;
	/*std::cout << "Searched " <<  nodeCount << " positions in " << mtime << " for " << nps << " nodes per second" << std::endl;
	std::cout << "Forced move " << moveToString(bestMove, c) << std::endl;
	std::cout << "Evaluation "  << eval << std::endl;*/

	return nodeCount;

}
userInterface* UI;
extern uint16_t killerMoves[20][2];

uint32_t searchMove(chessPosition* position, chessMove* bestMove, uint32_t maximal_time, uint32_t* nodeCount, uint64_t* mtime, int32_t* eval, bool doAspiration = true) {
	memset(killerMoves,0, 20*2*sizeof(uint16_t));
	resetNodes();
	resetQuiescenceNodes();
	resetIndices();
	resetqIndices();
	resetCalled();
	resetSortCalled();
	resetqCalled();
	resetSortqCalled();
	uint64_t start_ts  = get_timestamp();
	uint16_t depth = 6;
	/*uint64_t qnodes = 0;
	uint64_t nodes = 0;*/
	*eval = 0;
	int32_t alpha = -100005;
	int32_t beta  = 100005;
	while((get_timestamp()-start_ts < maximal_time) && (depth < 14)) {
		*eval = negamax(position, depth, alpha, beta, bestMove);


		if(doAspiration) {
			if ((*eval <= alpha) || (*eval >= beta)) {
				//std::cout << "Aspiration window search failed, researching..." <<std::endl;
				*eval = negamax(position, depth, -100005, 100005, bestMove);
			}

			alpha = *eval-50;
			beta  = *eval+50;
		}

		//std::cout << depth <<std::endl;
		*nodeCount = getNodes()+getQuiescenceNodes();
		//qnodes = getQuiescenceNodes();
		//nodes  = getNodes();
		*mtime = get_timestamp()-start_ts;
		/*double nps = ((double) *nodeCount)/((double) *mtime)*1000.0;
		std::cout << "Searched " <<  *nodeCount << " positions in " << *mtime << " for " << nps << " nodes per second" << std::endl;*/
		UI->sendSearchInfo(*nodeCount, *mtime, *eval, depth, moveToString(*bestMove, *position));
		depth++;
		if(*eval > 90000) {
			break;
		}
	}
	depth--;
	//std::cout << "Depth searched " << depth << std::endl;
	*mtime = get_timestamp()-start_ts;


	/*std::cout << "Qnodes " <<  qnodes << " normal nodes " << nodes <<std::endl;
	std::cout << "Forced move " << moveToString(*bestMove, *position) << std::endl;
	std::cout << "Evaluation "  << eval << std::endl;
	std::cout << "Negamax called " << getCalled() << " with depth >0; sort called " << getSortCalled() << " times for ratio " << ((double) getSortCalled())/((double) getCalled()) << std::endl;
	std::cout << "Quiescence called " << getqCalled() << " with depth >0; sort called " << getSortqCalled() << " times for ratio " << ((double) getSortqCalled())/((double) getqCalled()) << std::endl;

	int32_t* indices = getIndexCounts();
	for(uint16_t k=0; k < 50; k++){
		std::cout << indices[k] << " ";
	}
	std::cout << std::endl;
	int32_t* qindices = getqIndexCounts();
	for(uint16_t k=0; k < 50; k++){
		std::cout << qindices[k] << " ";
	}
	std::cout << std::endl;*/
	return depth;
}

bool checkMove(chessPosition& position, std::string move){
	vdt_vector<chessMove> moves = vdt_vector<chessMove>(100);
		uint64_t mv = stringToMove(move);
		generateAllMoves(&moves, &position);
		orderStandardMoves(&position, &moves, 0);
		std::sort(moves.data, moves.data+moves.length);
		bool found = false;
		chessMove m;
		for(uint16_t ind=0; ind < moves.length; ind++) {
			if(moves[ind].sortEval < -10000) {
				break;
			}
			if(moves[ind].move == mv){
				found = true;
				m = moves[ind];
				break;
			}
			if(moves[ind].type == castlingKingside){
				if(position.toMove == white){
					if(move == "e1g1"){
						found = true;
						m = moves[ind];
						break;
					}

				}
				if(position.toMove == black){
					if(move == "e8g8"){
						found = true;
						m = moves[ind];
						break;
					}

				}
			}

			if(moves[ind].type == castlingQueenside){
				if(position.toMove == white){
					if(move == "e1c1"){
						found = true;
						m = moves[ind];
						break;
					}

				}
				if(position.toMove == black){
					if(move == "e8c8"){
						found = true;
						m = moves[ind];
						break;
					}

				}
			}

		}
		if(found){
			std::cout << moveToString(m, position) << std::endl;
			makeMove(&m, &position);
			uint16_t kingField = findLSB(position.pieceTables[1-position.toMove][king]);
			if(isFieldAttacked(&position, position.toMove, kingField)){
				//std::cout << "Illegal move!" << std::endl;
				undoMove(&position);
			} else {
				std::string newPosition = chessPositionToString(position);
				//userInterface->sendNewPosition(newPosition);
			}
		} else {
			std::cout << "Invalid move!" << std::endl;
		}
		moves.free_array();
		return found;
}



int32_t createDebugEvaluation(const chessPosition* position, VDTevaluation* evalDebug){
	int32_t eval = evaluation(position, -110000, 110000);
	int32_t debugEval = debugEvaluation(position, evalDebug);
	std::cout << "Eval " << eval << std::endl;
	std::cout << "DebugEval " << debugEval << std::endl;
	if(eval != debugEval){
		return 0;
	}
	return 1;
}

void sendNewPosition(chessPosition* position) {
	VDTevaluation evalDebug;
	int32_t valid = createDebugEvaluation(position, &evalDebug);

	std::string newPosition = chessPositionToString(*position);
	UI->sendNewPosition(newPosition);
	UI->sendDebugEval(evalDebug, valid);
}


int main() {

	/*ifstream with("/home/vabi/deltapruning2.txt");
	ifstream without("/home/vabi/deltapruning.txt");

	int withAsp, withoutAsp;
	int totalWith = 0;
	int totalWithout = 0;
	while(with >> withAsp){
		without >> withoutAsp;
		totalWith = totalWith+withAsp;
		totalWithout = totalWithout+withoutAsp;
		double ratio = ((double) withAsp)/((double) withoutAsp);
		std::cout << withAsp << " " << withoutAsp << " " << ratio << std::endl;

	}

	double ratio = ((double) totalWith)/((double) totalWithout);
	std::cout << totalWith << " " << totalWithout << " " << ratio << std::endl;
	return 0;*/


	for(uint32_t index=0; index < 7; index++) {
		for(uint32_t t=0; t < 2; t++) {
			for(uint32_t k=0; k < 64; k++) {
					completePieceTables[index][t][k] = ((uint16_t) (pieceTables[index][t][k]+figureValues[index])) |  ( ((uint16_t) (endGamepieceTables[index][t][k]+figureValues[index])) << 16);
			}
		}
	}

	srand (time(NULL));
	fillZobristHash();

	/*std::string positionStrTest = "000000K000000P0P0000000000000000000000000000000000000000bk000r0qw0000";
	chessPosition p = stringToChessPosition(positionStrTest);
	std::cout << evaluation(&p, -110000, 110000) << std::endl;
	return 0;*/
	for(int depth = 3; depth < 9; depth++){
		ifstream file;
		file.open("chesspositionsfixed.txt");
		std::string line;
		uint64_t negamaxNodes = 0;
		uint64_t qNodes = 0;

		uint32_t nodes = 0;
		while(std::getline(file, line)){
			//std::cout << line << std::endl;
			uint64_t nmNodes = 0;
			uint64_t qn = 0;
			uint64_t newNodes = runSinglePositionPerformanceTest(line, depth, &nmNodes, &qn, true);
			if(newNodes != qn+nmNodes){
				std::cout << "WTF???" << std::endl;
			}
			nodes = nodes + newNodes;
			//std::cout << newNodes << std::endl;
			//std::cout << nmNodes << std::endl;
			//std::cout << qn << std::endl;
			negamaxNodes = negamaxNodes+nmNodes;
			qNodes = qNodes+qn;

		}
		std::cout << "Depth " << depth  << " Nodes " << nodes << std::endl;
		std::cout << "negamaxnodes " <<  negamaxNodes << std::endl;
		std::cout << "qnodes " <<  qNodes << std::endl;
		file.close();

	}

	return 0;
	//runTests();
	/*std::string positionStrTest = "R000K00RP0PBBPPP00P00Q0p0000P000000P0000bn00pnp0p0pNqpb0r000k00rbKQkq";
	chessPosition positionT = stringToChessPosition(positionStrTest);
	vdt_vector<chessMove> moves = vdt_vector<chessMove>(100);

	generateAllMoves(&moves, &positionT);
	orderStandardMoves(&positionT, &moves);*/
	//return 0;
	//UI = new networkUserInterface();
	UI = new uciInterface();

	std::string positionstr = "RNBQKBNRPPPPPPPP00000000000000000000000000000000pppppppprnbqkbnrwKQkq";
	chessPosition position = stringToChessPosition(positionstr);



	std::string move;

	while(1){
		UI->readInput();


		std::vector<std::string> moveList = std::vector<std::string>();
		if(UI->receiveNewPosition(positionstr, moveList)){

			free_position(&position);
			position = stringToChessPosition(positionstr);

			for(std::string seg: moveList){
				//std::cout << seg << std::endl;
				if(!checkMove(position, seg)){
					std::cout << "Illegal move detected" << std::endl;
				}
			}

			//TODO: this does not work in UCI!!!
			//sendNewPosition(&position);
		}

		if(UI->positionRequested()){
			sendNewPosition(&position);
		}

		if(UI->receiveMove(move)){
			vdt_vector<chessMove> moves = vdt_vector<chessMove>(100);
			uint64_t mv = stringToMove(move);
			generateAllMoves(&moves, &position);
			orderStandardMoves(&position, &moves, 0);
			std::sort(moves.data, moves.data+moves.length);
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
					sendNewPosition(&position);
					chessMove bestMove;
					uint32_t nodeCount;
					uint64_t mtime;
					int32_t eval = 0;
					searchMove(&position, &bestMove, 2000, &nodeCount, &mtime, &eval);
					std::cout << eval << std::endl;
					makeMove(&bestMove, &position);
					sendNewPosition(&position);

				}
			} else {
				std::cout << "Invalid move!" << std::endl;
			}
			moves.free_array();
		}

		if (UI->receiveUndoMove()) {
			std::cout << "Got undo event!" << std::endl;
			undoMove(&position);
			sendNewPosition(&position);
		}

		if(UI->receiveForceMove()) {
			chessMove bestMove;
			uint32_t nodeCount;
			uint64_t mtime;
			int32_t eval = 0;
			searchMove(&position, &bestMove, 2000, &nodeCount, &mtime, &eval);

			makeMove(&bestMove, &position);
			sendNewPosition(&position);

		}

		std::string newPosition;
		if(UI->receiveAnalyze(newPosition)){
			chessMove bestMove;
			uint32_t nodeCount;
			uint64_t mtime;
			int32_t eval = 0;
			searchMove(&position, &bestMove, 4000, &nodeCount, &mtime, &eval);
			UI->sendBestMove(moveToString(bestMove, position));
		}


	}




}
