/*
 * networkUserInterface.cpp
 *
 *  Created on: Sep 28, 2016
 *      Author: vabi
 */



#include <communication/Network/network.h>
#include "networkUserInterface.hpp"



void networkUserInterface::initialize(){
	initialize_network("127.0.0.1", 9876);
}


void networkUserInterface::sendNewPosition(std::string position) {
	fsarray<uint8_t> raw_str = fsarray<uint8_t>(position.length());
	memcpy(raw_str.data, position.c_str(), 69);
	VDTstring str = VDTstring(raw_str);
	auto newPosMsg = std::unique_ptr<VMPchessPosition>(new VMPchessPosition(str));
	send_msg(std::move(newPosMsg), 0);
}


bool networkUserInterface::receiveMove(std::string& move) {
	receive_from_network();
	auto checkMove = VMP_receive<VMPcheckMove>();
	if(checkMove){
		move = std::string((char*) checkMove->moveString.chars.data, 4);
		return true;
	}
	return false;
}

bool networkUserInterface::receiveUndoMove(){
	receive_from_network();
	auto undo = VMP_receive<VMPundoMove>();
	if (undo) {
		return true;
	}
	return false;
}


bool networkUserInterface::receiveForceMove(){
	receive_from_network();
	auto forceMove =  VMP_receive<VMPforceMove>();
	if(forceMove) {
		return true;
	}
	return false;
}

bool networkUserInterface::receiveNewPosition(std::string& position, std::vector<std::string>& moves) {
	receive_from_network();
	auto newPosition = VMP_receive<VMPchessPosition>();
	if(newPosition){
		position = std::string((char*) newPosition->position.chars.data, 69);
		return true;
	}
	return false;

}

void networkUserInterface::sendSearchInfo(uint64_t nodes, uint32_t time, int32_t eval, uint32_t depth, std::string bestMove) {
	fsarray<uint8_t> raw_str = fsarray<uint8_t>(bestMove.length());
	memcpy(raw_str.data, bestMove.c_str(), bestMove.length());
	VDTstring str = VDTstring(raw_str);
	auto searchInfo = std::unique_ptr<VMPsearchInfo>(new VMPsearchInfo(nodes, time, depth, eval, str));
	send_msg(std::move(searchInfo), 0);
}

void networkUserInterface::sendDebugEval(VDTevaluation eval, int32_t valid) {

	auto debugEval = std::unique_ptr<VMPevalDebug>(new VMPevalDebug(eval, valid));
	send_msg(std::move(debugEval), 0);

}
