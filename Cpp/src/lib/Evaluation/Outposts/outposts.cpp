/*
 * outposts.cpp
 *
 *  Created on: Jun 29, 2018
 *      Author: vabi
 */


#include "lib/basics.hpp"
#include "lib/bitfiddling.h"
#include "parameters/parameters.hpp"
#include "lib/Attacks/attacks.hpp"
#include "lib/Evaluation/evaluation.hpp"



EvalComponentResult outposts(const chessPosition* position, const evalParameters* par, EvalMemory* evalMemory __attribute__((unused))) {
    uint64_t wPawns = position->pieceTables[white][pawn];
    uint64_t bPawns = position->pieceTables[black][pawn];
    uint64_t wKnights = position->pieceTables[white][knight];
    uint64_t bKnights = position->pieceTables[black][knight];
/*#ifdef EXPERIMENTAL
    uint64_t wPawnAttack = NORTHONE(WESTONE(wPawns) | EASTONE(wPawns));
    uint64_t bPawnAttack = SOUTHONE(WESTONE(bPawns) | EASTONE(bPawns));
#endif*/

    int16_t ret = 0;

    uint64_t wStops = NORTHONE(wPawns);

    uint64_t wPawnAttackSpan = northFill(WESTONE(wStops) | EASTONE(wStops));

/*#ifdef EXPERIMENTAL
    assert(par->outposts);

    uint64_t bKnightOutposts = bKnights & (~wPawnAttackSpan) & wStops;
    ret = ret-5*popcount(bKnightOutposts);
    bKnightOutposts = (bKnights  & bPawnAttack) & (~wPawnAttackSpan) & wStops;
    ret = ret-15*popcount(bKnightOutposts);
#else*/
    uint64_t bKnightOutposts = bKnights & (~wPawnAttackSpan) & wStops;
    ret = ret-par->outposts*popcount(bKnightOutposts);
//#endif
    uint64_t bStops = SOUTHONE(bPawns);

    uint64_t bPawnAttackSpan = southFill(WESTONE(bStops) | EASTONE(bStops));

/*#ifdef EXPERIMENTAL
     uint64_t wKnightOutposts = (wKnights & wPawnAttack) & (~bPawnAttackSpan) & bStops;
    ret = ret+15*popcount(wKnightOutposts);
     wKnightOutposts = wKnights & (~bPawnAttackSpan) & bStops;
    ret = ret+5*popcount(wKnightOutposts);
#else*/
    uint64_t wKnightOutposts = wKnights & (~bPawnAttackSpan) & bStops;
    ret = ret+par->outposts*popcount(wKnightOutposts);
//#endif
    EvalComponentResult result;
    result.common = ret;
    return result;
}
