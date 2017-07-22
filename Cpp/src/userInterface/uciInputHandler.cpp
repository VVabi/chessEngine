/*
 * uciInputHandler.cpp
 *
 *  Created on: Mar 9, 2017
 *      Author: vabi
 */



#include <iostream>
#include "userEvents.hpp"
#include <sstream>
#include <vector>
#include <list>
#include <map>

void split(const std::string &s, char delim, std::list<std::string> &elems) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}


std::list<std::string> split(const std::string &s, char delim) {
    std::list<std::string> elems;
    split(s, delim, elems);
    return elems;
}

std::map<std::string, uciInput> userEventList;

void initUserEvents() {
	userEventList["uci"] 				= uci;
	userEventList["debug"] 				= debug;
	userEventList["isready"] 			= isready;
	userEventList["setoption"] 			= setoption;
	userEventList["ucinewgame"] 		= ucinewgame;
	userEventList["position"] 			= position;
	userEventList["go"] 				= go;
	userEventList["stop"] 				= stop;
	userEventList["ponderhit"] 			= ponderhit;
	userEventList["quit"] 				= quit;
	userEventList["perft"] 				= perft;
	userEventList["eval"] 				= eval;
	userEventList["pawneval"] 			= pawnEval;
	userEventList["see"] 			    = see;
	userEventList["clear"] 			    = clear;
	userEventList["info"] 			    = info;
	userEventList["setevalparam"]       = setEvalParam;
	userEventList["searchtest"]         = performanceTests;
	userEventList["getmoveordering"]    = getMoveOrdering;
}

userEvent getNextUserEvent() {
	std::string input;
	std::getline(std::cin, input);
	std::list<std::string> in = split(input, ' ');
	std::string commandType = in.front();
	auto iterator = userEventList.find(commandType);

	uciInput inputType = invalid;
	if(iterator == userEventList.end()) {
		std::cout << "Invalid user input" << std::endl;
	} else {
		inputType = iterator->second;
	}
	in.pop_front();
	userEvent ev = userEvent(inputType, in);
	return ev;
}
