/*
 * outposts.cpp
 *
 *  Created on: Jun 29, 2018
 *      Author: vabi
 */


#include "lib/basics.hpp"
#include "lib/bitfiddling.h"
#include "parameters/parameters.hpp"

#define OUTPOSTVALUE 15

int16_t outposts(const chessPosition* position) {
    uint64_t wPawns = position->pieceTables[white][pawn];
    uint64_t bPawns = position->pieceTables[black][pawn];
    uint64_t wKnights = position->pieceTables[white][knight];
    uint64_t bKnights = position->pieceTables[black][knight];

    int16_t ret = 0;

    uint64_t wStops = NORTHONE(wPawns);

    uint64_t wPawnAttackSpan = northFill(WESTONE(wStops) | EASTONE(wStops));

    uint64_t bKnightOutposts = bKnights & (~wPawnAttackSpan) & wStops;

    ret = ret-OUTPOSTVALUE*popcount(bKnightOutposts);

    uint64_t bStops = SOUTHONE(bPawns);

    uint64_t bPawnAttackSpan = southFill(WESTONE(bStops) | EASTONE(bStops));

    uint64_t wKnightOutposts = wKnights & (~bPawnAttackSpan) & bStops;

    ret = ret+OUTPOSTVALUE*popcount(wKnightOutposts);
    return ret;
}