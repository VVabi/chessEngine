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
//input messages



#define ISREADY 1

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

std::ofstream debugLog;

inputMessage uciInput;
inputMessage readyInput;
inputMessage positionInput;
inputMessage goInput;
inputMessage positionRequest;

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

}

bool handleReady() {
	if(readyInput.valid ){
		readyInput.valid = false;
		std::cout << "readyok" << std::endl;
		return true;
	}

	return false;

}

void uciInterface::initialize(){
	debugLog.open("/home/vabi/debug.txt");
	uciInput.valid = false;
	readyInput.valid = false;
	positionInput.valid = false;
	goInput.valid = false;

	while(1){
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
	}

}

void uciInterface::sendBestMove(std::string move) {

	std::cout << "bestmove " << move <<std::endl;

}

void uciInterface::sendNewPosition(std::string position) {
	std::cout << "currentPosition " << position << std::endl;

}

bool uciInterface::receiveAnalyze(std::string& position){


	if(goInput.valid){

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
