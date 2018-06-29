/*
 * Rookfiles.cpp
 *
 *  Created on: Jun 29, 2018
 *      Author: vabi
 */

#include "lib/basics.hpp"
#include "lib/bitfiddling.h"
#include "parameters/parameters.hpp"

int32_t rookOpenFiles(const chessPosition* position, uint8_t* pawnOccupancy, const evalParameters* evalParams) {
    int32_t ret = 0;

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
    return ret;
}
