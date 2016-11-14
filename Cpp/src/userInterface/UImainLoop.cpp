/*
 * UIlayer.cpp
 *
 *  Created on: Oct 9, 2016
 *      Author: vabi
 */


#include <lib/basics.hpp>
#include <userInterface/userInterface.hpp>
#include <userInterface/networkUserInterface.hpp>
#include <userInterface/uciInterface.hpp>
#include <fstream>
#include <lib/moveMaking/moveMaking.hpp>
#include <lib/Evaluation/evaluation.hpp>
#include "UIlayer.hpp"
#include <Search/search.hpp>
#include <iomanip>
userInterface* UI;

extern uint8_t searchId;
extern uint16_t killerMoves[20][2];
//#define UCI


uint32_t searchMove(chessPosition* position, chessMove* bestMove, uint32_t maximal_time, uint32_t* nodeCount, uint64_t* mtime, int32_t* eval, bool doAspiration) {
	memset(killerMoves,0, 20*2*sizeof(uint16_t));
	resetSearchData();
	resetQuiescenceNodes();
	uint64_t start_ts  = get_timestamp();
	uint16_t depth = 3;
	/*uint64_t qnodes = 0;
	uint64_t nodes = 0;*/
	*eval = 0;
	int32_t alpha = -32000;
	int32_t beta  = 32000;
	searchId++;
	uint64_t searchedNodes = 0;
	/*uint64_t goalNodes = 1800*maximal_time;
	while((searchedNodes < goalNodes) && (depth < 31)) {*/
	while(depth < 7){ //hacked to get repeatable results - there is a major bug hiding somewhere
		*eval = negamax(position, 0, depth+3, depth, alpha, beta, bestMove, true, false);

		if(doAspiration) {
			if ((*eval <= alpha) || (*eval >= beta)) {
				//std::cout << "Aspiration window search failed, researching..." <<std::endl;
				*eval = negamax(position, 0, depth+3, depth, -32000, 32000, bestMove, true, false);
			}

			alpha = *eval-50;
			beta  = *eval+50;
		}

		//std::cout << depth <<std::endl;

		//qnodes = getQuiescenceNodes();
		//nodes  = getNodes();
		*mtime = get_timestamp()-start_ts;
		/*double nps = ((double) *nodeCount)/((double) *mtime)*1000.0;
		std::cout << "Searched " <<  *nodeCount << " positions in " << *mtime << " for " << nps << " nodes per second" << std::endl;*/
		uint64_t totalNodes=0;
		searchDebugData data = getSearchData();
		for(uint16_t ind=0; ind < 25; ind++){
#ifndef UCI
			std::cout << ind << "  " << data.nodes[ind] << std::endl;
			std::cout << std::setw(16) << " ";

			for(int k=0; k < 50; k++){
				std::cout << std::setw(9) << data.bestIndex[ind][k];
			}
			std::cout << std::endl;
#endif
			totalNodes = totalNodes+data.nodes[ind];
		}
#ifndef UCI

		std::cout << "SEARCH DEBUG DATA" << std::endl;
		std::cout << "negamax called: "  << data.called << std::endl;
		std::cout << "Went to quiescence: " << data.wentToQuiescence << std::endl;
		std::cout << "By depht:"         << std::endl;

		std::cout << "Total " <<totalNodes << std::endl;
		std::cout << "Threefold repetition hashhits " << data.fake_3fold_repetitions+data.threefold_repetitions << std::endl;
		std::cout << "Actual repetitions " << data.threefold_repetitions << std::endl;
		std::cout << "Ratio "              << ((float) data.threefold_repetitions)/((float) data.fake_3fold_repetitions+data.threefold_repetitions) << std::endl;
		std::cout << "Futility tries "     << data.futility_tried <<std::endl;
		std::cout << "Successful futility " << data.futility_successful << std::endl;
		std::cout << "Nullmove tries "     << data.nullMovePruningTried <<std::endl;
		std::cout << "Successful nullmove " << data.nullMovePruningSuccessful << std::endl;
		std::cout << "Went to search "     << data.wentToSearch <<std::endl;
		std::cout << "Had to sort " << data.neededSort << std::endl;
#endif

		*nodeCount = (data.totalNodes+getQuiescenceNodes());
		UI->sendSearchInfo(*nodeCount, *mtime, *eval, depth, moveToString(*bestMove, *position));
		depth++;
		searchedNodes = searchedNodes+*nodeCount;
		if(*eval > 29000) {
			break;
		}

		if((*eval > 500) && (depth > 14)) {
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

void UIloop() {
#ifdef UCI
	UI = new uciInterface();
#else
	UI = new networkUserInterface();
#endif
	/*std::string positionstr = "RNBQKBNRPPPPPPPP00000000000000000000000000000000pppppppprnbqkbnrwKQkq";
	chessPosition position = stringToChessPosition(positionstr);*/
	std::string positionstr = " ";
	std::string fen = "4k3/2bp4/8/8/5q2/1B2R3/7P/NR5K b - - 0 1";
	chessPosition position = FENtoChessPosition(fen);

	chessMove mv;
	mv.sourceField = 29;
	mv.targetField = 15;
	mv.move = BIT64(29) | BIT64(15);
	mv.type = queenMove;
	mv.captureType = pawn;
	makeMove(&mv, &position);
	std::cout << SEE(&position, &mv) << std::endl;
	undoMove(&position);






	std::string move;
	while(1){
		UI->readInput();
		std::vector<std::string> moveList = std::vector<std::string>();
		if(UI->receiveNewPosition(positionstr, moveList)){
			free_position(&position);
#ifdef UCI
			position = FENtoChessPosition(positionstr);
#else
			position = stringToChessPosition(positionstr);
#endif
			for(std::string seg: moveList){
				//std::cout << seg << std::endl;
				if(!checkAndMakeMove(position, seg)){
					std::cout << "Illegal move detected" << std::endl;
				}
			}
			//TODO: change the network interface such that this works without the define
			#ifndef UCI
			sendNewPosition(&position);
			#endif
		}

		if(UI->positionRequested()){
			sendNewPosition(&position);
		}

		if(UI->receiveMove(move)){
			if(checkAndMakeMove(position, move)){
				sendNewPosition(&position);
				/*chessMove bestMove;
				uint32_t nodeCount;
				uint64_t mtime;
				int32_t eval = 0;
				searchMove(&position, &bestMove, 2000, &nodeCount, &mtime, &eval);
				std::cout << eval << std::endl;
				makeMove(&bestMove, &position);*/
				sendNewPosition(&position);
			}
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
			searchMove(&position, &bestMove, 4000, &nodeCount, &mtime, &eval);
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



