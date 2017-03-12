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
#include "interfaceStructs.hpp"
#include <lib/basics.hpp>

void memoryLibrarianAdd(std::string newPosition, std::vector<std::string> moves);
chessPosition memoryLibrarianRetrievePosition();

#endif /* USERINTERFACE_USERINTERFACE_HPP_ */
