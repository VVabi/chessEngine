/*
 * userInterface.hpp
 *
 *  Created on: Sep 28, 2016
 *      Author: vabi
 */

#ifndef USERINTERFACE_USERINTERFACE_HPP_
#define USERINTERFACE_USERINTERFACE_HPP_

#include <string>

class userInterface {
	public:
		virtual void initialize()=0;
		virtual void sendNewPosition(std::string newPosition)=0;
		virtual bool receiveMove(std::string& move)=0;
		virtual bool receiveUndoMove()=0;
		virtual bool receiveForceMove()=0;
		virtual~ userInterface(){};
};


#endif /* USERINTERFACE_USERINTERFACE_HPP_ */
