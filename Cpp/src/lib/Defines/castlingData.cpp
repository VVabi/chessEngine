/*
 * castlingData.cpp
 *
 *  Created on: Sep 9, 2016
 *      Author: vabi
 */



#include"boardParts.hpp"
#include <stdint.h>

uint64_t castlingBlockers[2][2] = {
        {WHITEKINGSIDECASTLEBLOCKINGFIELDS, WHITEQUEENSIDECASTLEBLOCKINGFIELDS},
        {BLACKKINGSIDECASTLEBLOCKINGFIELDS, BLACKQUEENSIDECASTLEBLOCKINGFIELDS}
};

