/*
 * interfaceStructs.hpp
 *
 *  Created on: Nov 22, 2016
 *      Author: vabi
 */

#ifndef USERINTERFACE_INTERFACESTRUCTS_HPP_
#define USERINTERFACE_INTERFACESTRUCTS_HPP_

#include <stdint.h>

struct searchParameters {
	int16_t maxDepth;
	int32_t maxTime;
	int32_t totalTime[2];
	int32_t increment[2];
};


#endif /* USERINTERFACE_INTERFACESTRUCTS_HPP_ */
