/*
 * interfaceStructs.hpp
 *
 *  Created on: Nov 22, 2016
 *      Author: vabi
 */

#ifndef USERINTERFACE_INTERFACESTRUCTS_HPP_
#define USERINTERFACE_INTERFACESTRUCTS_HPP_

#include <stdint.h>
#include "userEvents.hpp"

enum searchType {unknown, fixed_time, time_until_move, fixed_depth, infinite};

struct searchParameters {
    searchType type;
    int16_t depth;
    int32_t fixedTime;
    int32_t totalTime[2];
    int32_t increment[2];
    uint16_t movesToGo;
};

#endif /* USERINTERFACE_INTERFACESTRUCTS_HPP_ */
