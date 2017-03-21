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
#include <tests/tests.hpp>
#include <mutex>
#include <algorithm>
#include <hashTables/hashTables.hpp>
#include <lib/bitfiddling.h>
//necessary to get windows compile to run
//----------------------------------------
#include "mingw.thread.h"
#include "mingw.mutex.h"
#include <list>
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

std::mutex m;

void putLine(std::string output) {
	m.lock();
	std::cout << output << std::endl;
	m.unlock();
}


searchParameters paramsToUse;
std::atomic<bool> continueSearch;

uint32_t calcSearchTime(searchParameters params,  playerColor toMove, uint16_t numMadeMoves, uint32_t* worst_case_time) {

	if(params.type == infinite) {
		*worst_case_time = UINT32_MAX;
		return UINT32_MAX;
	}

	if(params.type == fixed_depth) {
		*worst_case_time = UINT32_MAX;
		return UINT32_MAX;
	}

	//TODO: implement the movesToGo parameter handling - currently we play stuff like "40 moves in 40 min" extremely badly
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
		float timeAllotted = completeExpectedTime/(remainingMoves);

		if(timeAllotted > total/10.0){
			timeAllotted = total/10.0;
		}
		*worst_case_time = total/10.0;
		return timeAllotted;
	}

	return 100; //some default
}

void sendSearchInfo(uint64_t nodes, uint32_t time, int32_t eval, uint32_t depth, std::list<std::string>& PV){
	double nps = ((double) nodes)/((double) time)*1000.0;
	uint64_t npsInt = nps;
	std::stringstream out;


	if(eval > 29000) {
		int16_t mate_in = (30000-eval+1)/2;
		out << "info depth " << depth << " mate " << mate_in << " nps " << npsInt << " nodes " << nodes << " pv ";
	} else if(eval < -29000) {
		int16_t mate_in = (-30000-eval)/2;
		out << "info depth " << depth << " mate " << mate_in << " nps " << npsInt << " nodes " << nodes << " pv ";
	} else {
		out << "info depth " << depth << " score cp " << eval << " nps " << npsInt << " nodes " << nodes << " pv ";
	}

	for(auto iterator = PV.begin(); iterator != PV.end(); iterator++) {
		out << *iterator << " ";
	}
	putLine(out.str());
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

/*#ifdef EXPERIMENTAL
std::ofstream fenLogger;
#endif*/

uint32_t searchMove(chessPosition* position, chessMove* bestMove, uint32_t* nodeCount, uint64_t* mtime, int32_t* eval, bool doAspiration, searchParameters params) {
/*#ifdef EXPERIMENTAL

	fenLogger.open("fen.txt", std::ios::app);

#endif*/
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
	vdt_vector<chessMove> moves = vdt_vector<chessMove>(150);
	generateAllMoves(&moves, position);
	uint16_t refutationTarget = 0;
	calculateStandardSortEvals(position, &moves, 0, 0, getHashMove(position->zobristHash), refutationTarget);
	std::stable_sort(moves.data, moves.data+moves.length);
	/*for(uint16_t ind=0; ind < moves.length; ind++) {
		std::cout<< moveToString(moves[ind], *position) << " Eval " << moves[ind].sortEval << std::endl;
	}*/

	pvLine line;
	line.numMoves = 0;
	while(checkContinue(params, depth, get_timestamp()-start_ts, totalTime)) {
		try{
			//std::cout << "Depth " << depth << std::endl;

			*eval = negamax(position, 0, depth+3, depth, alpha, beta, &line, true, false);
			if(doAspiration) {
				if ((*eval <= alpha) || (*eval >= beta)) {

					*eval = negamax(position, 0, depth+3, depth, -32000, 32000, &line, true, false);
				}

				alpha = *eval-50;
				beta  = *eval+50;
			}

			*bestMove = line.line[0];
			/*chessMove localBestMove;

			uint64_t* nodeCounts = new uint64_t[moves.length];
			for(uint16_t ind=0; ind < moves.length; ind++) {
				nodeCounts[ind] = 0;
			}


			*eval =  root_search(position, &localBestMove, alpha, beta, depth, depth+3, &moves, nodeCounts);
			if(doAspiration) {
				if ((*eval <= alpha) || (*eval >= beta)) {
					for(uint16_t ind=0; ind < moves.length; ind++) {
						nodeCounts[ind] = 0;
					}
					*eval = root_search(position, &localBestMove, -32000, 32000, depth, depth+3, &moves, nodeCounts);
				}

				alpha = *eval-50;
				beta  = *eval+50;
			}

			*bestMove = localBestMove;

			uint64_t maxCount = 0;

			for(uint16_t ind=0; ind < moves.length; ind++) {

				if(nodeCounts[ind] > maxCount){
					maxCount = nodeCounts[ind];
				}

			}
			uint32_t scaling = 1;

			if(maxCount > INT16_MAX) {
				scaling = maxCount/10000+1;
			}
			for(uint16_t ind=0; ind < moves.length; ind++) {

				if(nodeCounts[ind] > 0) {
					moves[ind].sortEval = (nodeCounts[ind]/scaling);
				} else {
					if(moves[ind].sortEval > 0) {
						moves[ind].sortEval = moves[ind].sortEval-500;
					}
				}

				if(moves[ind].move == bestMove->move) {
					moves[ind].sortEval = INT16_MAX;
				}
				//std::cout << moves[ind].sortEval << std::endl;
			}

			std::stable_sort(moves.data, moves.data+moves.length);*/

			/*std::cout << "New move list" << std::endl;

			for(uint16_t ind=0; ind < moves.length; ind++) {
				std::cout<< moveToString(moves[ind], *position) << " Eval " << moves[ind].sortEval << std::endl;
			}*/

			//TODO: free MEMORY!!!


		} catch(timeoutException& e) {
			//std::cout << "Search timed out" << std::endl;
			break;
		}

		*mtime = get_timestamp()-start_ts;
		uint64_t totalNodes=0;
		searchDebugData data = getSearchData();
		for(uint16_t ind=0; ind < 25; ind++){
			totalNodes = totalNodes+data.nodes[ind];
		}

		*nodeCount = (data.totalNodes+getQuiescenceNodes());

		std::list<std::string> moveList;

		for(uint16_t ind=0; ind < line.numMoves; ind++) {
			std::string mv = moveToString(line.line[ind]);
			moveList.push_back(mv);
		}


		sendSearchInfo(*nodeCount, *mtime, *eval, depth, moveList);
		//logSearch(*nodeCount, *mtime, *eval, depth, moveToString(*bestMove));
		line.numMoves = 0;
		depth++;
		searchedNodes = searchedNodes+*nodeCount;
		if(*eval > 29000) {
			break;
		}
	}

/*#ifdef EXPERIMENTAL
	fenLogger << chessPositionToFenString(*position, false) << std::endl;
	fenLogger.close();
#endif*/
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
std::atomic<bool> isSearching(false);

void search(chessPosition cposition, searchParameters params){
	chessMove bestMove;
	uint32_t nodeCount = 0;
	uint64_t mtime = 0;
	int32_t eval = 0;
	searchMove(&cposition, &bestMove,&nodeCount, &mtime, &eval, false, params);
	putLine("bestmove " + moveToString(bestMove));
	free_position(&cposition);
	isSearching = false;
}

void launchSearch() {
	continueSearch = true;
	if(engineThread.joinable()) {
		engineThread.join();
	}
	isSearching = true;
	chessPosition cposition = memoryLibrarianRetrievePosition();
	engineThread = std::thread(search, cposition, paramsToUse);
}



void handleStop() {
	continueSearch = false;
	if(engineThread.joinable()) {
		engineThread.join();
	}
	assert(!isSearching);
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
				putLine( "Missing parameter for " + current);
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
				putLine( "Missing parameter for " + current);
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
				putLine( "Missing parameter for " + current);
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
				putLine( "Missing parameter for " + current);
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
				putLine( "Missing parameter for " + current);
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
				putLine( "Missing parameter for " + current);
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
				putLine( "Missing parameter for " + current);
				params.type = unknown;
				break;
			}
			std::string togo =*iterator;
			params.movesToGo = StringToNumber<int32_t>(togo);
			params.type = time_until_move;
		}

	}


	/*std::cout << params.type << std::endl;

	if(params.type == time_until_move) {
		std::cout << "White time "      << params.totalTime[0] << std::endl;
		std::cout << "Black time "      << params.totalTime[1] << std::endl;
		std::cout << "White increment " << params.increment[0] << std::endl;
		std::cout << "Black increment " << params.increment[1] << std::endl;
		std::cout << "Moves to go "     << params.movesToGo    << std::endl;
	}*/

	if(params.type != unknown) {
		setSearchParams(params);
		launchSearch();
	}

}

void handlePerft(std::list<std::string> input){

		auto iterator = input.begin();

		if(iterator != input.end()) {
			uint16_t depth = StringToNumber<int32_t>(*iterator);
			chessPosition p = memoryLibrarianRetrievePosition();
			uint32_t perftret = perftNodes(&p, depth);
			free_position(&p);
			std::stringstream ret;
			ret << "perftresult " << perftret;
			putLine(ret.str());
		}
}


void handlePosition(std::list<std::string> input) {

	std::string fen = "";

	if(input.empty()) {
		//std::cout << "Invalid position request" << std::endl;
		return;
	}
	auto iterator = input.begin();
	if("startpos" == *iterator){
		fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
		iterator++;
	} else {
		if(*iterator != "fen") {
			putLine("Invalid position request");
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

	memoryLibrarianAdd(fen, moveList);
}

void handleEval() {

	chessPosition cposition = memoryLibrarianRetrievePosition();
	int32_t eval = evaluation(&cposition, -32000, 32000);
	evaluationResult res = getEvaluationResult();
	std::stringstream evalInfo;
	evalInfo << "Total " << eval;
	evalInfo << " Material " << cposition.figureEval;
	evalInfo << " PSQ " << res.PSQ-cposition.figureEval;
	evalInfo << " King safety " <<  res.kingSafety;
	evalInfo << " Mobility " << res.mobility;
	evalInfo << " Pawns " << res.staticPawn;
	evalInfo << " Passed pawns " << res.passedPawn;
	evalInfo << " rook open files " << res.rookOpenFiles;
	evalInfo << " bishoppair " << res.bishoppair;
	evalInfo << " Trapped pieces " << res.trappedPieces;
	evalInfo << " Outposts " << res.outPosts;
	putLine(evalInfo.str());
	free_position(&cposition);
}


void handlePawnEval() {
	chessPosition cposition = memoryLibrarianRetrievePosition();
	uint8_t pawnOcc[2];
	std::cout << staticPawnEvalComplete(&cposition, pawnOcc) << std::endl;
	free_position(&cposition);
}

void UIloop() {
	initUserEvents();
	bool continueLoop = true;
	while(continueLoop) {
		userEvent ev = getNextUserEvent();

		//restricted interface during search
		//------------------------------------
		if(isSearching) {
			switch(ev.input) {
				case isready:
					handleIsReady(std::cout);
					break;
				case stop:
					handleStop();
					break;
				case quit:
					handleStop();
					continueLoop = false;
					break;
				default:
					putLine("Invalid request during search");
			}

		} else {
			//full interface otherwise
			//---------------------------
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
				case pawnEval:
					handlePawnEval();
					break;
				case invalid:
					putLine("Invalid request");
					break;
				default:
					putLine("Not yet implemented");
			}
		}
	}

}



