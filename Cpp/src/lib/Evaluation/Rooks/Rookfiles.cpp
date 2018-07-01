/*
 * Rookfiles.cpp
 *
 *  Created on: Jun 29, 2018
 *      Author: vabi
 */

#include "lib/basics.hpp"
#include "lib/bitfiddling.h"
#include "parameters/parameters.hpp"
#include "lib/Attacks/attacks.hpp"
#include "lib/Evaluation/evaluation.hpp"

EvalComponentResult rookOpenFiles(const chessPosition* position, const evalParameters* evalParams, const AttackTable* attackTables  __attribute__ ((unused))) {
    int32_t ret = 0;

    uint8_t pawnOccupancy[2];

    for (uint8_t color=0; color < 2; color++) {
        uint64_t pawns = position->pieceTables[color][pawn];
        pawns = northFill(pawns);
        pawns = southFill(pawns);
        pawnOccupancy[color] = (uint8_t) (pawns & 0xFF);
    }

    for (uint8_t color = 0; color < 2; color++) {
        uint64_t rooks = position->pieceTables[color][rook];
        while (rooks) {
            uint16_t field = popLSB(rooks);
            uint16_t file = FILE(field);
            if ((pawnOccupancy[color] & (1 << file)) == 0) {
                ret = ret+(1-2*color)*evalParams->rookOnOpenFile;
            }
        }
    }
#ifdef DEBUG
    if ((ret > 100) || (ret < -100)) {
        logError("Rook open file value too large: "+ret);
    }
#endif
    EvalComponentResult result;
#ifdef EXPERIMENTAL
    result.common = (4*ret)/10;
#else
    result.common = ret;
#endif
    return result;
}
