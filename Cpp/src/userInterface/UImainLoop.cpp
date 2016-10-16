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

extern uint16_t killerMoves[20][2];
//#define UCI


uint32_t searchMove(chessPosition* position, chessMove* bestMove, uint32_t maximal_time, uint32_t* nodeCount, uint64_t* mtime, int32_t* eval, bool doAspiration) {
	memset(killerMoves,0, 20*2*sizeof(uint16_t));
	resetSearchData();
	uint64_t start_ts  = get_timestamp();
	uint16_t depth = 3;
	/*uint64_t qnodes = 0;
	uint64_t nodes = 0;*/
	*eval = 0;
	int32_t alpha = -100005;
	int32_t beta  = 100005;

	while((get_timestamp()-start_ts < maximal_time) && (depth < 21)) {

		*eval = negamax(position, depth, alpha, beta, bestMove);

		if(doAspiration) {
			if ((*eval <= alpha) || (*eval >= beta)) {
				//std::cout << "Aspiration window search failed, researching..." <<std::endl;
				*eval = negamax(position, depth, -100005, 100005, bestMove);
			}

			alpha = *eval-50;
			beta  = *eval+50;
		}
		searchDebugData data = getSearchData();
		//std::cout << depth <<std::endl;

		//qnodes = getQuiescenceNodes();
		//nodes  = getNodes();
		*mtime = get_timestamp()-start_ts;
		/*double nps = ((double) *nodeCount)/((double) *mtime)*1000.0;
		std::cout << "Searched " <<  *nodeCount << " positions in " << *mtime << " for " << nps << " nodes per second" << std::endl;*/

#ifndef UCI
		std::cout << "SEARCH DEBUG DATA" << std::endl;
		std::cout << "negamax called: "  << data.called << std::endl;
		std::cout << "Went to quiescence: " << data.wentToQuiescence << std::endl;
		std::cout << "By depht:"         << std::endl;
		uint64_t totalNodes=0;
		for(uint16_t ind=0; ind < 25; ind++){
			std::cout << ind << "  " << data.nodes[ind] << std::endl;
			std::cout << std::setw(16) << std::endl;
			for(int k=0; k < 50; k++){
				std::cout << std::setw(9) << data.bestIndex[ind][k];
			}
			std::cout << std::endl;
			totalNodes = totalNodes+data.nodes[ind];
		}
		std::cout << "Total " <<totalNodes << std::endl;
		std::cout << "Threefold repetition hashhits " << data.fake_3fold_repetitions+data.threefold_repetitions << std::endl;
		std::cout << "Actual repetitions " << data.threefold_repetitions << std::endl;
		std::cout << "Ratio "              << ((float) data.threefold_repetitions)/((float) data.fake_3fold_repetitions+data.threefold_repetitions) << std::endl;
		std::cout << "Futility tries "     << data.futility_tried <<std::endl;
		std::cout << "Successful futility " << data.futility_successful << std::endl;
#endif
		*nodeCount = (totalNodes+getQuiescenceNodes());
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
			//TODO: change the network interface such that this works without the define
			#ifndef UCI
			sendNewPosition(&position);
			#endif
		}

		if(UI->positionRequested()){
			sendNewPosition(&position);
		}

		if(UI->receiveMove(move)){
			if(checkMove(position, move)){
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



