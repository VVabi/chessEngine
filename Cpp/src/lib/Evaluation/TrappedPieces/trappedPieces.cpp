/*
 * trappedPieces.cpp
 *
 *  Created on: Jun 29, 2018
 *      Author: vabi
 */

#include "lib/basics.hpp"
#include "lib/bitfiddling.h"
#include "parameters/parameters.hpp"

int16_t trappedPieces(const chessPosition* position) {
    uint64_t wPawns = position->pieceTables[white][pawn];
    uint64_t bPawns = position->pieceTables[black][pawn];
    uint64_t wKnights = position->pieceTables[white][knight];
    uint64_t bKnights = position->pieceTables[black][knight];
    uint64_t wBishops = position->pieceTables[white][bishop];
    uint64_t bBishops = position->pieceTables[black][bishop];
    uint64_t wRooks = position->pieceTables[white][rook];
    uint64_t bRooks = position->pieceTables[black][rook];
    int16_t ret = 0;


    if (wBishops & H7) {
        if ((bPawns & G6) && (bPawns & F7)) {
            ret = ret-50;
        }
    }

    if (wBishops & A7) {
        if ((bPawns & B6) && (bPawns & C7)) {
            ret = ret-50;
        }
    }

    if (bBishops & H2) {
        if ((wPawns & G3) && (wPawns & F2)) {
            ret = ret+50;
        }
    }

    if (bBishops & A2) {
        if ((wPawns & B3) && (wPawns & C2)) {
            ret = ret+50;
        }
    }

    if (wKnights & H8) {
        if ((bPawns & (H7 | F7))) {
            ret = ret-50;
        }
    }

    if (wKnights & A8) {
        if ((bPawns & (A7 | C7))) {
            ret = ret-50;
        }
    }

    if (bKnights & H1) {
        if ((wPawns & (H2 | F2))) {
            ret = ret+50;
        }
    }

    if (bKnights & A1) {
        if ((wPawns & (A2 | C2))) {
            ret = ret+50;
        }
    }

    if (bRooks & H8) {
        if ((bPawns & (H7 | H6)) && (position->pieceTables[black][king] & G8)) {
            ret = ret+50;
        }
    }

    if (bRooks & A8) {
        if ((bPawns & (A7 | A6)) && (position->pieceTables[black][king] & B8)) {
            ret = ret+50;
        }
    }

    if (wRooks & H1) {
        if ((wPawns & (H2 | H3)) && (position->pieceTables[white][king] & G1)) {
            ret = ret-50;
        }
    }

    if (wRooks & A1) {
        if ((wPawns & (A2 | A3)) && (position->pieceTables[white][king] & B1)) {
            ret = ret-50;
        }
    }
    return ret;
}
