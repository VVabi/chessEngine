/*
 * UIlayer.cpp
 *
 *  Created on: Oct 9, 2016
 *      Author: vabi
 */


#include <lib/basics.hpp>
#include <userInterface/userInterface.hpp>
#include <userInterface/uciInterface.hpp>
#include <fstream>
#include <lib/moveMaking/moveMaking.hpp>
#include <lib/Evaluation/evaluation.hpp>
#include "UIlayer.hpp"
#include <Search/search.hpp>
#include <iomanip>
#include <userInterface/interfaceStructs.hpp>
#include <lib/moveGeneration/moveGeneration.hpp>
#include <logging/logger.hpp>
#include <iostream>
#include "userEvents.hpp"
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <atomic>
#include <thread>
#include "mingw.thread.h"
#include <tests/tests.hpp>

extern uint8_t searchId;
extern uint16_t killerMoves[20][2];

void handleUciInput(std::ostream& stream) {
	stream << "id name Vabi" << std::endl;
	stream << "id author Fabian Lenhardt" << std::endl;
	stream << "uciok" << std::endl;
}

void handleIsReady(std::ostream& stream) {
	stream << "readyok" <<std::endl;
}

template <typename T>
T StringToNumber ( const std::string &Text )//Text not by const reference so that the function can be used with a
{                               //character array as argument
	std::stringstream ss(Text);
	T result;
	return ss >> result ? result : 0;
}





searchParameters paramsToUse;
std::atomic<bool> continueSearch;
chessPosition cposition;

uint32_t calcSearchTime(searchParameters params,  playerColor toMove, uint16_t numMadeMoves, uint32_t* worst_case_time) {

	if(params.type == infinite) {
		*worst_case_time = UINT32_MAX;
		return UINT32_MAX;
	}

	if(params.type == fixed_depth) {
		*worst_case_time = UINT32_MAX;
		return UINT32_MAX;
	}

	/*if(depth < 7){
		return true;
	} else {
		return false;
	}*/
	if(params.totalTime[toMove] > 0) {
		uint32_t total = params.totalTime[toMove];
		uint32_t increment = params.increment[toMove];

		//estimate remaining moves
		uint16_t totalExpectedMoves = 1/2*numMadeMoves+60;

		int16_t remainingMoves = totalExpectedMoves-numMadeMoves;

		if(remainingMoves < 20){
			remainingMoves = 20;
		}

		uint32_t completeExpectedTime = total+remainingMoves*increment;
		std::cout << remainingMoves << std::endl;
		std::cout << completeExpectedTime << std::endl;
		float timeAllotted = completeExpectedTime/(2.0*remainingMoves);

		if(timeAllotted > total/10.0){
			timeAllotted = total/10.0;
		}
		*worst_case_time = total/10.0;
		return timeAllotted;
	}

	return 100; //some default
}

void sendSearchInfo(uint64_t nodes, uint32_t time, int32_t eval, uint32_t depth, std::string bestMove){
	double nps = ((double) nodes)/((double) time)*1000.0;
	uint64_t npsInt = nps;
	std::cout << "info depth " << depth << " score cp " << eval << " nps " << npsInt << " nodes " << nodes << " pv " << bestMove << std::endl;
}


bool checkContinue(searchParameters params, uint16_t depth, uint16_t passedTime, uint16_t allottedTime) {

	if(depth > 27) {
		return false;
	}

	if(params.type == infinite) {
		return true;
	}

	if(params.type == fixed_depth) {
		return (params.depth >= depth) || (depth <= 3);
	}

	if(params.type == time_until_move) {
		return passedTime < allottedTime;
	}

	if(params.type == fixed_time) {
		return passedTime < params.fixedTime;
	}

	return true;
}

uint32_t searchMove(chessPosition* position, chessMove* bestMove, uint32_t* nodeCount, uint64_t* mtime, int32_t* eval, bool doAspiration, searchParameters params) {
	memset(killerMoves,0, 20*2*sizeof(uint16_t));
	resetSearchData();
	resetQuiescenceNodes();
	uint64_t start_ts  = get_timestamp();
	uint16_t depth = 3;
	*eval = 0;
	int32_t alpha = -32000;
	int32_t beta  = 32000;
	searchId++;
	uint64_t searchedNodes = 0;
	uint32_t worst_case_search_time = 0;
	uint32_t totalTime = calcSearchTime(params, position->toMove, position->madeMoves.length, &worst_case_search_time);
	setTotalTime(worst_case_search_time, start_ts);
	uint16_t madeMoves = position->madeMoves.length;
	while(checkContinue(params, depth, get_timestamp()-start_ts, totalTime)) {
		try{
			//std::cout << "Depth " << depth << std::endl;
			chessMove localBestMove;
			*eval = negamax(position, 0, depth+3, depth, alpha, beta, &localBestMove, true, false);
			if(doAspiration) {
				if ((*eval <= alpha) || (*eval >= beta)) {
					std::cout << "Aspiration window search failed, researching..." <<std::endl;
					*eval = negamax(position, 0, depth+3, depth, -32000, 32000, &localBestMove, true, false);
				}

				alpha = *eval-50;
				beta  = *eval+50;
			}

			*bestMove = localBestMove;
		} catch(timeoutException& e) {
			std::cout << "Search timed out" << std::endl;
			break;
		}

		*mtime = get_timestamp()-start_ts;
		uint64_t totalNodes=0;
		searchDebugData data = getSearchData();
		for(uint16_t ind=0; ind < 25; ind++){
			totalNodes = totalNodes+data.nodes[ind];
		}

		*nodeCount = (data.totalNodes+getQuiescenceNodes());
		sendSearchInfo(*nodeCount, *mtime, *eval, depth, moveToString(*bestMove, *position));
		logSearch(*nodeCount, *mtime, *eval, depth, moveToString(*bestMove, *position));
		depth++;
		searchedNodes = searchedNodes+*nodeCount;
		if(*eval > 29000) {
			break;
		}
	}
	depth--;
	*mtime = get_timestamp()-start_ts;
	while(position->madeMoves.length > madeMoves){
		chessMove current = position->madeMoves[position->madeMoves.length-1];

		if(current.move == 0) {
			undoNullMove(position);
		} else {
			undoMove(position);
		}
	}
	return depth;
}



void setSearchParams(searchParameters params) {
	paramsToUse = params;
}

std::thread engineThread;

void search(chessPosition p, searchParameters params){
	std::cout << "Thread started" <<std::endl;
	chessMove bestMove;
	uint32_t nodeCount = 0;
	uint64_t mtime = 0;
	int32_t eval = 0;
	searchMove(&p, &bestMove,&nodeCount, &mtime, &eval, false, params);
	std::cout << "bestmove " << moveToString(bestMove, p) << std::endl;
	std::cout << "Leaving thread" <<std::endl;
}

void launchSearch() {
	continueSearch = true;
	if(engineThread.joinable()) {
		engineThread.join();
	}
	engineThread = std::thread(search, cposition, paramsToUse);
	std::cout << "Launched thread" <<std::endl;
}



void handleStop() {
	continueSearch = false;
	if(engineThread.joinable()) {
		engineThread.join();
	}
	std::cout << "Joined engine thread" << std::endl;
}

void handleGo(std::list<std::string> input) {

	searchParameters params;
	params.type = unknown;
	params.depth = 0;
	params.fixedTime = 0;
	params.totalTime[0] = 0;
	params.totalTime[1] = 0;
	params.increment[0] = 0;
	params.increment[1] = 0;
	params.movesToGo = UINT16_MAX;


	for(auto iterator = input.begin(), end = input.end(); iterator != end; ++iterator) {
		std::string current = *iterator;
		if("wtime" == current) {
			iterator++;
			if(iterator == input.end()) {
				std::cout << "Missing parameter for " << current << std::endl;
				params.type = unknown;
				break;
			}
			std::string time = *iterator;
			params.totalTime[0] = StringToNumber<int32_t>(time);
			params.type = time_until_move;
		}
		if("btime" == current) {
			iterator++;
			if(iterator == input.end()) {
				std::cout << "Missing parameter for " << current << std::endl;
				params.type = unknown;
				break;
			}
			std::string time = *iterator;
			params.totalTime[1] = StringToNumber<int32_t>(time);
			params.type = time_until_move;
		}
		if("winc" == current) {
			iterator++;
			if(iterator == input.end()) {
				std::cout << "Missing parameter for " << current << std::endl;
				params.type = unknown;
				break;
			}
			std::string time = *iterator;
			params.increment[0] = StringToNumber<int32_t>(time);
			params.type = time_until_move;
		}
		if("binc" == current) {
			iterator++;
			if(iterator == input.end()) {
				std::cout << "Missing parameter for " << current << std::endl;
				params.type = unknown;
				break;
			}
			std::string time = *iterator;
			params.increment[1] = StringToNumber<int32_t>(time);
			params.type = time_until_move;
		}

		if("infinite" == current) {
			params.type = infinite;
		}

		if("movetime" == current) {
			iterator++;
			if(iterator == input.end()) {
				std::cout << "Missing parameter for " << current << std::endl;
				params.type = unknown;
				break;
			}
			std::string time = *iterator;
			params.fixedTime = StringToNumber<int32_t>(time);
			params.type = fixed_time;
		}

		if("depth" == current) {
			iterator++;
			if(iterator == input.end()) {
				std::cout << "Missing parameter for " << current << std::endl;
				params.type = unknown;
				break;
			}
			std::string d =  *iterator;
			params.depth = StringToNumber<int32_t>(d);
			params.type = fixed_depth;
		}


		if("movestogo" == current) {
			iterator++;
			if(iterator == input.end()) {
				std::cout << "Missing parameter for " << current << std::endl;
				params.type = unknown;
				break;
			}
			std::string togo =*iterator;
			params.movesToGo = StringToNumber<int32_t>(togo);
			params.type = time_until_move;
		}

	}


	std::cout << params.type << std::endl;

	if(params.type == time_until_move) {
		std::cout << "White time "      << params.totalTime[0] << std::endl;
		std::cout << "Black time "      << params.totalTime[1] << std::endl;
		std::cout << "White increment " << params.increment[0] << std::endl;
		std::cout << "Black increment " << params.increment[1] << std::endl;
		std::cout << "Moves to go "     << params.movesToGo    << std::endl;
	}

	if(params.type != unknown) {
		setSearchParams(params);


		launchSearch();
	}

}

void handlePerft(std::list<std::string> input){

		auto iterator = input.begin();

		if(iterator != input.end()) {
			uint16_t depth = StringToNumber<int32_t>(*iterator);
			uint32_t perftret = perftNodes(&cposition, depth);
			std::cout << "Perft result " << perftret << std::endl;

		}
}


void handlePosition(std::list<std::string> input) {

	std::string fen = "";

	if(input.empty()) {
		std::cout << "Invalid position request" << std::endl;
		return;
	}
	auto iterator = input.begin();
	if("startpos" == *iterator){
		fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
		iterator++;
	} else {
		if(*iterator != "fen") {
			std::cout << "Invalid position request" << std::endl;
			return;
		}
		iterator++;
		fen = *iterator;
		iterator++;
		while((iterator != input.end()) && ("moves" != *iterator)) {
			if("fen" != *iterator) {
			fen = fen+" "+*iterator;
			}
			iterator++;
		}

	}



	std::vector<std::string> moveList;
	while(iterator != input.end()) {
		if("moves" !=*iterator) {
			moveList.push_back(*iterator);
		}
		iterator++;
	}

	std::cout << fen << std::endl;

	for(auto iterator = moveList.begin(); iterator != moveList.end(); iterator++) {
		std::cout << *iterator << std::endl;
	}

	free_position(&cposition);
	cposition = FENtoChessPosition(fen);
	for(std::string seg: moveList){
		//std::cout << seg << std::endl;
		if(!checkAndMakeMove(cposition, seg)){
			std::cout << "Illegal move detected" << std::endl;
		}
	}

}

void handleEval() {
	int32_t eval = evaluation(&cposition, -32000, 32000);
	evaluationResult res = getEvaluationResult();
	std::cout << "Total " << eval;
	std::cout << " Material " << cposition.figureEval;
	std::cout << " PSQ " << res.PSQ-cposition.figureEval;
	std::cout << " King safety " <<  res.kingSafety;
	std::cout << " Mobility " << res.mobility;
	std::cout << " Pawns " << res.staticPawn;
	std::cout << " Passed pawns " << res.passedPawn;
	std::cout << " rook open files " << res.rookOpenFiles;
	std::cout << " bishoppair " << res.bishoppair;
	std::cout << std::endl;
}

void UIloop() {
	initUserEvents();
	bool continueLoop = true;
	while(continueLoop) {
		userEvent ev = getNextUserEvent();

		switch(ev.input) {
			case uci:
				handleUciInput(std::cout);
				break;
			case isready:
				handleIsReady(std::cout);
				break;
			case go:
				handleGo(ev.data);
				break;
			case position:
				handlePosition(ev.data);
				break;
			case stop:
				handleStop();
				break;
			case quit:
				handleStop();
				continueLoop = false;
				break;
			case perft:
				handlePerft(ev.data);
				break;
			case eval:
				handleEval();
				break;
			default:
				std::cout << "Not yet implemented" << std::endl;
		}
	}

}



