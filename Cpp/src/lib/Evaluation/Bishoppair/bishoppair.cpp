/*
 * bishoppair.cpp
 *
 *  Created on: Jun 29, 2018
 *      Author: vabi
 */

#include "lib/basics.hpp"
#include "lib/bitfiddling.h"
#include "parameters/parameters.hpp"
#include "lib/Attacks/attacks.hpp"

int16_t bishopPair(const chessPosition* position, const evalParameters* par, const AttackTable* attackTables __attribute__ ((unused))) {
    int16_t ret = 0;

    uint64_t numWhiteBishops = popcount(position->pieceTables[white][bishop]);
    if (numWhiteBishops > 1) {
        ret  = ret+par->bishoppair;
    }

    uint64_t numblackBishops = popcount(position->pieceTables[black][bishop]);
    if (numblackBishops > 1) {
        ret = ret-par->bishoppair;
    }
    return ret;
}
