/*
 * staticPawnEvaluation.cpp
 *
 *  Created on: Mar 13, 2017
 *      Author: vabi
 */

#include <lib/basics.hpp>
#include "lib/Evaluation/evaluation.hpp"
#include <lib/bitfiddling.h>
#include <lib/Defines/boardParts.hpp>
#include <parameters/parameters.hpp>
#include <hashTables/hashTables.hpp>
#include <iostream>

void getBackwardsPawns(uint64_t* whiteBackwards, uint64_t* blackBackwards, uint64_t wPawns, uint64_t bPawns) {
    uint64_t wStops = NORTHONE(wPawns);
    uint64_t wPawnFill = northFill(wStops);
    uint64_t wPawnAttackSpan = WESTONE(wPawnFill) | EASTONE(wPawnFill);
    uint64_t wPawnAttacks = WESTONE(wStops) | EASTONE(wStops);
    uint64_t bStops = SOUTHONE(bPawns);
    uint64_t bPawnFill = southFill(bStops);
    uint64_t bPawnAttackSpan = WESTONE(bPawnFill) | EASTONE(bPawnFill);
    uint64_t bPawnAttacks = WESTONE(bStops) | EASTONE(bStops);
    *whiteBackwards = SOUTHONE(wStops & bPawnAttacks & ~wPawnAttackSpan);
    *blackBackwards = NORTHONE(bStops & wPawnAttacks & ~bPawnAttackSpan);
}


uint8_t getPawnColumns(uint64_t in) {
    return getColumnOcc(in);
}

uint64_t getDoubledPawns(uint64_t in) {
    uint64_t stops = NORTHONE(in);
    uint64_t pawnFill = northFill(stops);
    uint64_t frontDoubled =  pawnFill & in;
    return (in & (southFill(frontDoubled) | northFill(frontDoubled)));
}

uint64_t getIsolatedPawns(uint64_t in) {
    uint64_t fill = northFill(in) | southFill(in);
    uint64_t emptyCols = ~fill;
    uint64_t westEmpty = EASTONE(emptyCols);
    uint64_t eastEmpty = WESTONE(emptyCols);
    uint64_t neighborsEmpty = (westEmpty | FILEA) & (eastEmpty | FILEH);
    return in & neighborsEmpty;
}


int16_t isolatedPawnTable[] = { 0, 0, 0, 0, 0, 0, 0, 0,
                                -5, -6, -7, -8, -8, -7, -6, -5,
                                -5, -6, -8, -9, -9, -8, -6, -5,
                                -7, -9, -11, -13, -13, -11, -9, -7,
                                -7, -9, -11, -13, -13, -11, -9, -7,
                                -5, -6, -8, -9, -9, -8, -6, -5,
                                -5, -6, -7, -8, -8, -7, -6, -5,
                                 0, 0, 0, 0, 0, 0, 0, 0,
};

int16_t staticPawnEvalComplete(const chessPosition* position) {
    //from the pov of WHITE
    int16_t ev = 0;

    uint64_t doubledPawns[2];
    uint64_t isolatedPawns[2];
    uint64_t backwardsPawns[2];
    //uint64_t columnFill[2];
    uint64_t neighboringColumn[2];
    uint64_t frontColumnFill[2];

    uint64_t wStops = NORTHONE(position->pieceTables[white][pawn]);
    uint64_t wPawnFill = northFill(wStops);
    uint64_t wPawnAttackSpan = WESTONE(wPawnFill) | EASTONE(wPawnFill);
    //uint64_t wPawnAttacks = WESTONE(wStops) | EASTONE(wStops);
    uint64_t bStops = SOUTHONE(position->pieceTables[black][pawn]);
    uint64_t bPawnFill = southFill(bStops);
    uint64_t bPawnAttackSpan = WESTONE(bPawnFill) | EASTONE(bPawnFill);
    //uint64_t bPawnAttacks = WESTONE(bStops) | EASTONE(bStops);

    doubledPawns[white] = getDoubledPawns(position->pieceTables[white][pawn]);
    doubledPawns[black] = getDoubledPawns(position->pieceTables[black][pawn]);
    isolatedPawns[white] = getIsolatedPawns(position->pieceTables[white][pawn]);
    isolatedPawns[black] = getIsolatedPawns(position->pieceTables[black][pawn]);
    getBackwardsPawns(backwardsPawns, backwardsPawns+1, position->pieceTables[white][pawn], position->pieceTables[black][pawn]);

    /*columnFill[white]  = southFill(position->pieceTables[white][pawn]) | northFill(position->pieceTables[white][pawn]);
    columnFill[black]  = southFill(position->pieceTables[black][pawn]) | northFill(position->pieceTables[black][pawn]);*/
/*
    neighboringColumn[white] = WESTONE(columnFill[white]) | EASTONE(columnFill[white]);
    neighboringColumn[black] = WESTONE(columnFill[black]) | EASTONE(columnFill[black]);
*/
    neighboringColumn[white] = wPawnAttackSpan;
    neighboringColumn[black] = bPawnAttackSpan;

    frontColumnFill[white] = NORTHONE(position->pieceTables[white][pawn]);
    frontColumnFill[black] = SOUTHONE(position->pieceTables[black][pawn]);

    uint64_t wDouble = doubledPawns[white];
    while (wDouble) {
        ev = ev-10;
        uint16_t field = popLSB(wDouble);
        //std::cout << "White double pawn on " << field << " " << ev <<  std::endl;
        if (!(BIT64(field) & neighboringColumn[black])) { //no black pawn on an adjacent column
            ev = ev-5;
            //std::cout << "White double pawn unresolvable on " << field << " " << ev <<  std::endl;
        }
    }



    uint64_t bDouble = doubledPawns[black];
    while (bDouble) {
        ev = ev+10;
        uint16_t field = popLSB(bDouble);
        //std::cout << "Black double pawn on " << field << " " << ev <<  std::endl;
        if (!(BIT64(field) & neighboringColumn[white])) { //no black pawn on an adjacent column
            ev = ev+5;
            //std::cout << "Black double pawn unresolvable on " << field << " " << ev <<  std::endl;
        }
    }

    /*uint64_t wDouble = doubledPawns[white];
        while (wDouble) {
            ev = ev-10;
            uint16_t field = popLSB(wDouble);
            std::cout << "White double pawn on " << field << " " << ev <<  std::endl;
            if (!(BIT64(field) & neighboringColumn[black])) { //no black pawn on an adjacent column
                ev = ev-5;
                std::cout << "White double pawn unresolvable on " << field << " " << ev <<  std::endl;
            }
        }



        uint64_t bDouble = doubledPawns[black];
        while (bDouble) {
            ev = ev+10;
            uint16_t field = popLSB(bDouble);
            std::cout << "Black double pawn on " << field << " " << ev <<  std::endl;
            if (!(BIT64(field) & neighboringColumn[white])) { //no black pawn on an adjacent column
                ev = ev+5;
                std::cout << "Black double pawn unresolvable on " << field << " " << ev <<  std::endl;
            }
        }*/


    uint64_t wIso = isolatedPawns[white];
    while (wIso) {
        uint16_t field = popLSB(wIso);
        ev = ev+isolatedPawnTable[field];
        //std::cout << "White iso pawn on " << field << " ev " << isolatedPawnTable[field]<< " " << ev <<  std::endl;
    }

    uint64_t bIso = isolatedPawns[black];
    while (bIso) {
        uint16_t field = popLSB(bIso);
        ev = ev-isolatedPawnTable[field];
        //std::cout << "Black iso pawn on " << field << " ev " << isolatedPawnTable[field]<< " " << ev <<  std::endl;
    }

    uint64_t wBackwards = backwardsPawns[white];
    while (wBackwards) {
        uint16_t field = popLSB(wBackwards);
        if (!(BIT64(field) & frontColumnFill[black])) {
            ev = ev-5;
            //std::cout << "White backwards pawn on " << field << " " << ev <<  std::endl;
        }
    }

    uint64_t bBackwards = backwardsPawns[black];
    while (bBackwards) {
        uint16_t field = popLSB(bBackwards);
        if (!(BIT64(field) & frontColumnFill[white])) {
            ev = ev+5;
            //std::cout << "Black backwards pawn on " << field << " " << ev <<  std::endl;
        }
    }


    //reward pawns side by side. Needs further testing; looks questionable actually
    //---------------------------------
/*#ifdef EXPERIMENTAL
    uint64_t wpawns = position->pieceTables[white][pawn];
    uint64_t wsideByside = (wpawns & WESTONE(wpawns)) | (wpawns & WESTONE(wpawns));
    ev = ev+2*popcount(wsideByside);

    uint64_t bpawns = position->pieceTables[black][pawn];
    uint64_t bsideByside = (bpawns & WESTONE(bpawns)) | (bpawns & WESTONE(bpawns));
    ev = ev-2*popcount(bsideByside);
#endif*/
    return ev;
}
