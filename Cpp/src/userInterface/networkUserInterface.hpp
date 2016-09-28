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

class networkUserInterface: public userInterface {
	public:
		networkUserInterface(){
			initialize();
		}
		void initialize();
		void sendNewPosition(std::string position);
		bool receiveMove(std::string& move);
		bool receiveUndoMove();
		bool receiveForceMove();
		bool receiveNewPosition(std::string& position);
};

#endif /* USERINTERFACE_NETWORKUSERINTERFACE_HPP_ */
