/*
 * userInterface.hpp
 *
 *  Created on: Sep 28, 2016
 *      Author: vabi
 */

#ifndef USERINTERFACE_USERINTERFACE_HPP_
#define USERINTERFACE_USERINTERFACE_HPP_

#include <string>
#include <vector>
#include <communication/gen/VDT.h>
#include "interfaceStructs.hpp"

class userInterface {
	public:
		virtual void initialize()=0;
		virtual void sendNewPosition(std::string newPosition)=0;
		virtual void sendSearchInfo(uint64_t nodes, uint32_t time, int32_t eval, uint32_t depth, std::string bestMove)=0;
		virtual bool receiveMove(std::string& move)=0;
		virtual bool receiveUndoMove()=0;
		virtual bool receiveForceMove()=0;
		virtual bool receiveNewPosition(std::string& position, std::vector<std::string>& moves)=0;
		virtual~ userInterface(){};
		virtual void readInput()=0;
		virtual void sendBestMove(std::string position)=0;
		virtual bool receiveAnalyze(std::string& position, searchParameters& params)=0;
		virtual bool positionRequested()=0;
		virtual void sendDebugEval(VDTevaluation eval, int32_t valid)=0;
};


#endif /* USERINTERFACE_USERINTERFACE_HPP_ */
