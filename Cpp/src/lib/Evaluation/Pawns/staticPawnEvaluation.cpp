/*
 * staticPawnEvaluation.cpp
 *
 *  Created on: Mar 13, 2017
 *      Author: vabi
 */

#include <DataTypes/data_utils.hpp>
#include <lib/basics.hpp>
#include "lib/Evaluation/evaluation.hpp"
#include <lib/bitfiddling.h>
#include <lib/Defines/boardParts.hpp>
#include <parameters/parameters.hpp>
#include <hashTables/hashTables.hpp>
#include <iostream>

void getBackwardsPawns(uint64_t* whiteBackwards, uint64_t* blackBackwards,
        uint64_t wPawns, uint64_t bPawns) {
    uint64_t wStops             = NORTHONE(wPawns);
    uint64_t wPawnFill          = northFill(wStops);
    uint64_t wPawnAttackSpan    = WESTONE(wPawnFill) | EASTONE(wPawnFill);
    uint64_t wPawnAttacks       = WESTONE(wStops) | EASTONE(wStops);
    uint64_t bStops             = SOUTHONE(bPawns);
    uint64_t bPawnFill          = southFill(bStops);
    uint64_t bPawnAttackSpan    = WESTONE(bPawnFill) | EASTONE(bPawnFill);
    uint64_t bPawnAttacks       = WESTONE(bStops) | EASTONE(bStops);
    *whiteBackwards             = SOUTHONE(wStops & bPawnAttacks & ~wPawnAttackSpan); //TODO: currently a blocked pawn can be backwards - that is strange
    *blackBackwards             = NORTHONE(bStops & wPawnAttacks & ~bPawnAttackSpan);
}

uint8_t getPawnColumns(uint64_t in) {
    return getColumnOcc(in);
}

uint64_t getDoubledPawns(uint64_t in) {
    uint64_t stops          = NORTHONE(in);
    uint64_t pawnFill       = northFill(stops);
    uint64_t frontDoubled   = pawnFill & in;
    return (in & (southFill(frontDoubled) | northFill(frontDoubled)));
}

uint64_t getIsolatedPawns(uint64_t in) {
    uint64_t fill           = northFill(in) | southFill(in);
    uint64_t emptyCols      = ~fill;
    uint64_t westEmpty      = EASTONE(emptyCols);
    uint64_t eastEmpty      = WESTONE(emptyCols);
    uint64_t neighborsEmpty = (westEmpty | FILEA) & (eastEmpty | FILEH);
    return in & neighborsEmpty;
}



EvalComponentResult doubledPawnEval(const chessPosition* position,
        const evalParameters* par,
        EvalMemory* evalMemory __attribute__((unused))) {
    int16_t ev = 0;
    uint64_t doubledPawns[2];
    doubledPawns[white] = getDoubledPawns(position->pieceTables[white][pawn]);
    doubledPawns[black] = getDoubledPawns(position->pieceTables[black][pawn]);
    uint64_t wDouble = doubledPawns[white];
    uint64_t neighboringColumn[2];

    uint64_t wStops = NORTHONE(position->pieceTables[white][pawn]);
    uint64_t wPawnFill = northFill(wStops);
    uint64_t wPawnAttackSpan = WESTONE(wPawnFill) | EASTONE(wPawnFill);

    uint64_t bStops = SOUTHONE(position->pieceTables[black][pawn]);
    uint64_t bPawnFill = southFill(bStops);
    uint64_t bPawnAttackSpan = WESTONE(bPawnFill) | EASTONE(bPawnFill);

    neighboringColumn[white] = wPawnAttackSpan;
    neighboringColumn[black] = bPawnAttackSpan;

    while (wDouble) {
        ev = ev + par->staticPawnParameters.doublePawn;
        uint16_t field = popLSB(wDouble);
        if (!(BIT64(field) & neighboringColumn[black])) { //no black pawn on an adjacent column
            ev = ev + par->staticPawnParameters.unresolvableDoublePawn;
        }
    }

    uint64_t bDouble = doubledPawns[black];
    while (bDouble) {
        ev = ev - par->staticPawnParameters.doublePawn;
        uint16_t field = popLSB(bDouble);
        if (!(BIT64(field) & neighboringColumn[white])) { //no black pawn on an adjacent column
            ev = ev - par->staticPawnParameters.unresolvableDoublePawn;
        }
    }
    EvalComponentResult ret;
    ret.common      = ev;
    ret.early_game  = 0;
    ret.endgame     = 0;
    return ret;
}

EvalComponentResult isolatedPawnEval(const chessPosition* position,
        const evalParameters* par,
        EvalMemory* evalMemory __attribute__((unused)))  {
    int16_t ev = 0;
    uint64_t isolatedPawns[2];
    isolatedPawns[white] = getIsolatedPawns(position->pieceTables[white][pawn]);
    isolatedPawns[black] = getIsolatedPawns(position->pieceTables[black][pawn]);
    uint64_t wIso = isolatedPawns[white];
    while (wIso) {
        uint16_t field = popLSB(wIso);
        ev = ev + SAFE_ARRAY_ACCESS(par->staticPawnParameters.isolatedPawnTable, field);
    }

    uint64_t bIso = isolatedPawns[black];
    while (bIso) {
        uint16_t field = popLSB(bIso);
        ev = ev - SAFE_ARRAY_ACCESS(par->staticPawnParameters.isolatedPawnTable, field);
    }
    EvalComponentResult ret;
    ret.common      = ev;
    ret.early_game  = 0;
    ret.endgame     = 0;
    return ret;
}

EvalComponentResult backwardPawnsEval(const chessPosition* position,
        const evalParameters* par,
        EvalMemory* evalMemory __attribute__((unused))) {
    int16_t ev = 0;
    uint64_t backwardsPawns[2];
    uint64_t frontColumnFill[2];

    getBackwardsPawns(backwardsPawns, backwardsPawns + 1,
            position->pieceTables[white][pawn],
            position->pieceTables[black][pawn]);

    frontColumnFill[white] = NORTHONE(position->pieceTables[white][pawn]);
    frontColumnFill[black] = SOUTHONE(position->pieceTables[black][pawn]);

    uint64_t wBackwards = backwardsPawns[white];
    while (wBackwards) {
        uint16_t field = popLSB(wBackwards);
        if (!(BIT64(field) & frontColumnFill[black])) {
            ev = ev + par->staticPawnParameters.backwardsPawn;
        }
    }

    uint64_t bBackwards = backwardsPawns[black];
    while (bBackwards) {
        uint16_t field = popLSB(bBackwards);
        if (!(BIT64(field) & frontColumnFill[white])) {
            ev = ev - par->staticPawnParameters.backwardsPawn;
        }
    }
    EvalComponentResult ret;
    ret.common      = ev;
    ret.early_game  = 0;
    ret.endgame     = 0;
    return ret;
}

