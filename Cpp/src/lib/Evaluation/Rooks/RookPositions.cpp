/*
 * RookPositions.cpp
 *
 *  Created on: Jan 2, 2019
 *      Author: vabi
 */

#include <lib/basics.hpp>
#include <lib/Evaluation/evaluation.hpp>
#include <parameters/parameters.hpp>


EvalComponentResult rookPositioning(const chessPosition* position, const evalParameters* evalParams __attribute__((unused)), EvalMemory* evalMemory) {
    EvalComponentResult ret;
    uint64_t wrooks             = position->pieceTables[white][rook];
    uint64_t wPassedPawns       = evalMemory->passedPawns[white];
    uint64_t brooks             = position->pieceTables[black][rook];
    uint64_t bPassedPawns       = evalMemory->passedPawns[black];

    uint64_t wPassedPawnsForward    = northFill(wPassedPawns);
    uint64_t wPassedPawnsBackward   = southFill(wPassedPawns);
    uint64_t badRooks               = wPassedPawnsForward & wrooks;
    uint64_t goodRooks              = wPassedPawnsBackward & wrooks;
    ret.endgame                    -= 5*popcount(badRooks);
    ret.endgame                    += 5*popcount(goodRooks);

    uint64_t bPassedPawnsForward    = southFill(bPassedPawns);
    uint64_t bPassedPawnsBackward   = northFill(bPassedPawns);
    badRooks                        = bPassedPawnsForward & brooks;
    goodRooks                       = bPassedPawnsBackward & brooks;
    ret.endgame                    += 5*popcount(badRooks);
    ret.endgame                    -= 5*popcount(goodRooks);
    return ret;
}
