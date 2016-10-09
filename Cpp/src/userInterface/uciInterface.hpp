/*
 * uciInterface.hpp
 *
 *  Created on: Oct 1, 2016
 *      Author: vabi
 */

#ifndef USERINTERFACE_UCIINTERFACE_HPP_
#define USERINTERFACE_UCIINTERFACE_HPP_

#include "userInterface.hpp"
#include <vector>
#include <communication/gen/VDT.h>
class uciInterface: public userInterface {
	public:
		uciInterface(){
			initialize();
		}
		void initialize();
		void sendNewPosition(std::string position);
		void sendSearchInfo(uint64_t nodes, uint32_t time, int32_t eval, uint32_t depth, std::string bestMove);
		bool receiveMove(std::string& move);
		bool receiveUndoMove();
		bool receiveForceMove();
		bool receiveNewPosition(std::string& position, std::vector<std::string>& moves);
		void readInput();
		void sendBestMove(std::string position);
		bool receiveAnalyze(std::string& position);
		bool positionRequested();
		void sendDebugEval(VDTevaluation eval, int32_t valid) { return; };
};




#endif /* USERINTERFACE_UCIINTERFACE_HPP_ */
