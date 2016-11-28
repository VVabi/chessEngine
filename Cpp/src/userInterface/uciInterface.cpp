/*
 * uciInterface.cpp
 *
 *  Created on: Oct 1, 2016
 *      Author: vabi
 */


#include <iostream>
#include "uciInterface.hpp"
#include <list>
#include <string>
#include <stdint.h>
#include <fstream>
#include <sstream>
#include <vector>
//input messages



#define ISREADY 1

void split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

struct inputMessage{
	std::string msg;
	bool valid;
};

std::list<inputMessage> inputs;

std::string readLine(){
	std::string response;
	std::getline(std::cin, response);
	return response;
}

template <typename T>
T StringToNumber ( const std::string &Text )//Text not by const reference so that the function can be used with a
{                               //character array as argument
	std::stringstream ss(Text);
	T result;
	return ss >> result ? result : 0;
}

std::ofstream debugLog;

inputMessage uciInput;
inputMessage readyInput;
inputMessage positionInput;
inputMessage goInput;
inputMessage positionRequest;

bool handleReady() {
	if(readyInput.valid ){
		readyInput.valid = false;
		std::cout << "readyok" << std::endl;
		return true;
	}

	return false;

}

void uciInterface::readInput(){
	std::string next = readLine();
	uint16_t pos=next.find(' ',0);
	std::string cmd = next.substr(0, pos);
	debugLog << "INPUT " << next << std::endl;
	if(cmd == "uci"){
		uciInput.msg = next;
		uciInput.valid = true;
	}

	if(cmd == "isready"){
		readyInput.msg = next;
		readyInput.valid = true;
	}

	if(cmd == "position"){
		positionInput.msg = next;
		positionInput.valid =true;
	}

	if(cmd == "go"){
		goInput.msg = next;
		goInput.valid = true;
	}

	if(cmd == "getPosition"){
		positionRequest.msg = next;
		positionRequest.valid = true;
	}

	handleReady();
	if(uciInput.valid){
		std::cout << "id name vabi" << std::endl;
		std::cout << "id author Fabian" << std::endl;
		std::cout << "uciok" << std::endl;
		uciInput.valid = false;
	}

}



void uciInterface::initialize(){
	debugLog.open("/home/vabi/debug.txt");
	uciInput.valid = false;
	readyInput.valid = false;
	positionInput.valid = false;
	goInput.valid = false;

	/*while(1){
		readInput();

		if(uciInput.valid){
			std::cout << "id name vabi" << std::endl;
			std::cout << "id author Fabian" << std::endl;
			std::cout << "uciok" << std::endl;
			uciInput.valid = false;
		}

		if(handleReady()){
			break;
		}
	}*/

}

void uciInterface::sendBestMove(std::string move) {

	std::cout << "bestmove " << move <<std::endl;

}

void uciInterface::sendNewPosition(std::string position) {
	std::cout << "currentPosition " << position << std::endl;

}

bool uciInterface::receiveAnalyze(std::string& position, searchParameters& params){


	if(goInput.valid){
		std::vector<std::string> splitted =  split(goInput.msg, ' ');
		params.maxDepth 	= -1;
		params.increment[0] = 0;
		params.increment[1] = 0;
		params.maxTime 		= 0;
		params.totalTime[0] = 0;
		params.totalTime[1] = 0;
		uint16_t index = 0;
		while(index < splitted.size()) {
			std::string current = splitted[index];

			if(current == "wtime"){
				index++;
				std::string whiteTime = splitted[index];
				params.totalTime[0] = StringToNumber<int32_t>(whiteTime);
			}

			if(current == "btime"){
				index++;
				std::string blackTime = splitted[index];
				params.totalTime[1] = StringToNumber<int32_t>(blackTime);
			}

			if(current == "winc"){
				index++;
				std::string whiteinc = splitted[index];
				params.increment[0] = StringToNumber<int32_t>(whiteinc);
			}

			if(current == "binc"){
				index++;
				std::string blackinc = splitted[index];
				params.increment[1] = StringToNumber<int32_t>(blackinc);
			}
			index++;
		}
		goInput.valid = false;
		return true;
	}
	return false;

}


bool uciInterface::positionRequested(){
	if(positionRequest.valid){
		positionRequest.valid = false;
		return true;
	}
	return false;
}

void uciInterface::sendSearchInfo(uint64_t nodes, uint32_t time, int32_t eval, uint32_t depth, std::string bestMove){
	double nps = ((double) nodes)/((double) time)*1000.0;
	uint64_t npsInt = nps;
	std::cout << "info depth " << depth << " score cp " << eval << " nps " << npsInt << " pv " << bestMove << std::endl;
}

bool uciInterface::receiveMove(std::string& move){
	return false;
}

bool uciInterface::receiveUndoMove() {
	return false;
}


bool uciInterface::receiveForceMove() {
	return false;

}



bool uciInterface::receiveNewPosition(std::string& position, std::vector<std::string>& moves) {

	if(positionInput.valid){
		positionInput.valid = false;

		position = "RNBQKBNRPPPPPPPP00000000000000000000000000000000pppppppprnbqkbnrwKQkq";

		std::string sub = positionInput.msg.substr(9,8);
		if(sub == "startpos"){
			position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
		} else {
			positionInput.msg = positionInput.msg.substr(13);
			position = positionInput.msg;
		}


		std::stringstream test(positionInput.msg);
		std::string segment;
		std::vector<std::string> seglist;

		while(std::getline(test, segment, ' '))
		{
		   seglist.push_back(segment);
		}

		if(seglist.size() > 2){
			bool atMoves = false;
			for(std::string seg: seglist){
				if((seg == "moves")){
					atMoves = true;
					continue;
				}
				if(atMoves){
					moves.push_back(seg);
				}
			}

		}


		return true;
	}

	return false;
}
