/*
 * threats.cpp
 *
 *  Created on: Feb 2, 2019
 *      Author: vabi
 */

#include <lib/basics.hpp>
#include <lib/basicTypes.hpp>
#include <lib/bitfiddling.h>
#include <lib/Evaluation/evaluation.hpp>
#include <cstdint>

EvalComponentResult threatDetection(const chessPosition* position, const evalParameters* evalParams, EvalMemory* evalMemory) {
    EvalComponentResult ret;
    for (uint16_t color=0; color < 2; color++) {
        uint64_t hangingPieces = position->pieces[color] & evalMemory->attackTables[INVERTCOLOR(color)].completeAttackTable & ~evalMemory->attackTables[color].completeAttackTable;

        uint64_t pieces = position->pieces[color]^position->pieceTables[color][pawn];
        hangingPieces = hangingPieces | (pieces & evalMemory->attackTables[INVERTCOLOR(color)].attackTables[pawn]);

        pieces = pieces^position->pieceTables[color][bishop]^position->pieceTables[color][knight];
        hangingPieces = hangingPieces | (pieces & (evalMemory->attackTables[INVERTCOLOR(color)].attackTables[knight] | evalMemory->attackTables[INVERTCOLOR(color)].attackTables[bishop]));

        pieces = pieces^position->pieceTables[color][rook];
        hangingPieces = hangingPieces | (pieces & (evalMemory->attackTables[INVERTCOLOR(color)].attackTables[rook]));

        if (popcount(hangingPieces) > 1) {
            ret.common += COLORSIGN(color)*evalParams->hangingPieces;
        }
    }
    return ret;
}
