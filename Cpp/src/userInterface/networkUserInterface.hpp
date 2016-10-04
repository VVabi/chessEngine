/*
 * networkUserInterface.hpp
 *
 *  Created on: Sep 28, 2016
 *      Author: vabi
 */

#ifndef USERINTERFACE_NETWORKUSERINTERFACE_HPP_
#define USERINTERFACE_NETWORKUSERINTERFACE_HPP_

#include "userInterface.hpp"
#include <communication/gen/VMPmessages.h>
#include <communication/gen/VDT.h>
#include <vector>
class networkUserInterface: public userInterface {
	public:
		networkUserInterface(){
			initialize();
		}
		void initialize();
		void sendNewPosition(std::string position);
		void sendSearchInfo(uint64_t nodes, uint32_t time, int32_t eval, uint32_t depth, std::string bestMove);
		bool receiveMove(std::string& move);
		bool receiveUndoMove();
		bool receiveForceMove();
		bool receiveNewPosition(std::string& position, std::vector<std::string>& moves);
		void readInput() {};
		void sendBestMove(std::string position) {};
		bool receiveAnalyze(std::string& position) { return false; }
		bool positionRequested() { return false;}
};

#endif /* USERINTERFACE_NETWORKUSERINTERFACE_HPP_ */
