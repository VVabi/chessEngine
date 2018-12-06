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
#include "lib/Evaluation/evaluation.hpp"

EvalComponentResult bishopPair(const chessPosition* position, const evalParameters* par, EvalMemory* evalMemory __attribute__((unused))) {
    int16_t ret = 0;

    uint64_t numWhiteBishops = popcount(position->pieceTables[white][bishop]);
    if (numWhiteBishops > 1) {
        ret  = ret+par->bishoppair;
    }

    uint64_t numblackBishops = popcount(position->pieceTables[black][bishop]);
    if (numblackBishops > 1) {
        ret = ret-par->bishoppair;
    }
    EvalComponentResult result;
    result.common = ret;

/*#ifdef EXPERIMENTAL


    if (numWhiteBishops == 1) {
        int16_t field = findLSB(position->pieceTables[white][bishop]);

        uint64_t otherColoredFields;
        uint64_t ownColouredFields;

        if (field & 1) {
            otherColoredFields = BLACKFIELDS;
            ownColouredFields  = WHITEFIELDS;
        } else {
            otherColoredFields = WHITEFIELDS;
            ownColouredFields  = BLACKFIELDS;
        }
        result.early_game += (popcount(otherColoredFields & position->pieceTables[white][pawn])-popcount(ownColouredFields & position->pieceTables[white][pawn]));
    }

    if (numblackBishops == 1) {
        int16_t field = findLSB(position->pieceTables[black][bishop]);

        uint64_t otherColoredFields;
        uint64_t ownColouredFields;

        if (field & 1) {
            otherColoredFields = BLACKFIELDS;
            ownColouredFields  = WHITEFIELDS;
        } else {
            otherColoredFields = WHITEFIELDS;
            ownColouredFields  = BLACKFIELDS;
        }
        result.early_game -= (popcount(otherColoredFields & position->pieceTables[black][pawn])-popcount(ownColouredFields & position->pieceTables[black][pawn]));
    }
#endif*/
    return result;
}
