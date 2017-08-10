/*
 * pawnEvaluation.cpp
 *
 *  Created on: Sep 25, 2016
 *      Author: vabi
 */

#include <algorithm>
#include <lib/basics.hpp>
#include "evaluation.hpp"
#include <lib/bitfiddling.h>
#include <lib/Defines/boardParts.hpp>
#include <parameters/parameters.hpp>
#include <hashTables/hashTables.hpp>

extern uint64_t files[];
extern uint64_t  passedPawnMasks[2][64];
extern uint16_t taperingValues[81];


static int16_t const passedPawnEvalValues[2][64] = {
        { 0, 0, 0, 0, 0, 0, 0, 0,
          8, 7, 6, 5, 5, 6, 7, 8,
          16, 15, 14, 13, 13, 14, 15, 16,
          31, 29, 27, 25, 25, 27, 29, 31,
          47, 44, 42, 40, 40, 42, 44, 47,
          90, 89, 84, 80, 80, 84, 89, 90,
          135, 131, 128, 125, 125, 128, 131, 135,
          0 , 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0,
         135, 131, 128, 125, 125, 128, 131, 135,
         90, 89, 84, 80, 80, 84, 89, 90,
          47, 44, 42, 40, 40, 42, 44, 47,
          31, 29, 27, 25, 25, 27, 29, 31,
          16, 15, 14, 13, 13, 14, 15, 16,
          8, 7, 6, 5, 5, 6, 7, 8,
          0, 0, 0, 0, 0, 0, 0, 0 }
};


static int16_t kingToPromotionFieldDistance[7][7] = {  // [dist to promotion][king distance from promotion field
        {0, 0, 0, 0, 0, 0, 0 }, //never happens
        {0, 0, -10, -20, -30, -45, -60},
        {0, 0, -5, -10, -15, -25, -40 },
        {0, 0, 0, -5, -10, -15, -25 },
        {0, 0, 0, 0, -5, -10, -15 },
        {0, 0, 0, 0, -5, -5, -10 },
        {0, 0, 0, 0, -5, -5, -10 },
};

static uint16_t distBetweenFields(uint16_t a, uint16_t b) {
    uint16_t fileA = FILE(a);
    uint16_t rowA  = ROW(a);
    uint16_t fileB = FILE(b);
    uint16_t rowB  = ROW(b);

    uint16_t rowDiff = (rowA > rowB? rowA-rowB: rowB-rowA);

    uint16_t fileDiff = (fileA > fileB? fileA-fileB : fileB-fileA);

    return std::max(rowDiff, fileDiff);
}

uint64_t staticpawncalls = 0;
uint64_t staticpawnhashhits = 0;

#define BLOCKEDDIVISOR 2


static int32_t passedPawnEval(int32_t* untaperedEval, uint64_t whitePawns, uint64_t blackPawns, uint16_t blackKing, uint16_t whiteKing, uint64_t whitePieces, uint64_t blackPieces) {
    //TODO: remove code duplication. Generally this code sucks - too many white/black diffs...
    int32_t eval = 0;
    uint64_t whitePawnBuffer = whitePawns;

    while (whitePawnBuffer) {
        uint16_t field = popLSB(whitePawnBuffer);
        if ((passedPawnMasks[white][field] & blackPawns) == 0) {
            uint16_t promotionField  = FILE(field)+56;
            uint16_t distToPromotion = 7-ROW(field);
            uint16_t kingDist        = distBetweenFields(promotionField, blackKing);
            bool blocked = blackPieces & BIT64(field+8);
            if (distToPromotion <= 2) {
                *untaperedEval = *untaperedEval+passedPawnEvalValues[white][field];
                                if (blocked) {
                                    *untaperedEval = *untaperedEval-passedPawnEvalValues[white][field]/BLOCKEDDIVISOR;
                                }
                            } else {
                eval = eval+passedPawnEvalValues[white][field];
                    if (blocked) {
                        eval = eval-passedPawnEvalValues[white][field]/BLOCKEDDIVISOR;
                    }
            }
/*#ifdef EXPERIMENTAL
            int16_t coverage = 0;
            if (distToPromotion < 3) {
                if (whiteAttackTable->attackTables[bishop] & BIT64(promotionField)) {
                    coverage = coverage+15;
                }

                uint64_t minorAttacks = blackAttackTable->attackTables[bishop] | blackAttackTable->attackTables[knight];

                if (minorAttacks & BIT64(promotionField)) {
                    coverage = coverage-15;
                }

                if (blackAttackTable->attackTables[rook] & BIT64(promotionField)) {
                    coverage = coverage-10;
                }

                if (blackAttackTable->attackTables[queen] & BIT64(promotionField)) {
                    coverage = coverage-5;
                }
            }
#endif*/

            eval = eval-kingToPromotionFieldDistance[distToPromotion][kingDist];
        }
    }
    uint64_t blackPawnBuffer = blackPawns;
    while (blackPawnBuffer) {
        uint16_t field = popLSB(blackPawnBuffer);
        if ((passedPawnMasks[black][field] & whitePawns) == 0) {
            uint16_t promotionField  = FILE(field);
            uint16_t distToPromotion = ROW(field);
            uint16_t kingDist        = distBetweenFields(promotionField, whiteKing);
            bool blocked = whitePieces & BIT64(field-8);
            if (distToPromotion <= 2) {
                *untaperedEval = *untaperedEval-passedPawnEvalValues[black][field];
                if (blocked) {
                    *untaperedEval = *untaperedEval+passedPawnEvalValues[black][field]/BLOCKEDDIVISOR;
                }
            } else {
                eval = eval-passedPawnEvalValues[black][field];
                    if (blocked) {
                        eval = eval+passedPawnEvalValues[black][field]/BLOCKEDDIVISOR;
                    }
            }
        /*#ifdef EXPERIMENTAL
            int16_t coverage = 0;
            if (distToPromotion < 3) {
                if (blackAttackTable->attackTables[bishop] & BIT64(promotionField)) {
                    coverage = coverage-15;
                }

                uint64_t minorAttacks = whiteAttackTable->attackTables[bishop] | whiteAttackTable->attackTables[knight];

                if (minorAttacks & BIT64(promotionField)) {
                    coverage = coverage+15;
                }

                if (whiteAttackTable->attackTables[rook] & BIT64(promotionField)) {
                    coverage = coverage+10;
                }

                if (whiteAttackTable->attackTables[queen] & BIT64(promotionField)) {
                    coverage = coverage+5;
                }

            }
            eval = eval+coverage;
            #endif*/
            eval  = eval+kingToPromotionFieldDistance[distToPromotion][kingDist];
        }
    }
    return eval;
}

int32_t staticPawnEval(uint64_t pawns, playerColor color, uint8_t* pawnColumnOccupancy, const staticPawnEvalParameters* staticPawnParameters) { //all stuff depending only on own pawn structure, not the opponents
    int32_t eval = 0;
    *pawnColumnOccupancy = 0;
    uint8_t doublePawns         = 0; //TODO: extend to triple pawns
    for (uint16_t ind = 0; ind <8; ind++) {
        uint64_t occ = pawns & files[ind];
        if (occ) {
            *pawnColumnOccupancy = *pawnColumnOccupancy | (1 << ind);
        }
        if (popcount(occ) > 1) {
            doublePawns = doublePawns | (1 << ind);
        }
    }

    uint8_t isolatedPawns = ~((*pawnColumnOccupancy << 1) | (*pawnColumnOccupancy >> 1))  & *pawnColumnOccupancy;


    uint64_t isolatedDoublePawns = isolatedPawns & doublePawns;

    uint64_t nonIsolatedDoublePawns =  (~isolatedPawns) & doublePawns;





    eval = eval+staticPawnParameters->isolatedDoublePawn*popcount(isolatedDoublePawns)+staticPawnParameters->nonIsolatedDoublePawn*popcount(nonIsolatedDoublePawns);

    eval = eval+staticPawnParameters->isolatedPawn*popcount(isolatedPawns & (~isolatedDoublePawns));

/*#ifdef EXPERIMENTAL
    //reward pawns covered by other pawns
    //---------------------------------------
    uint64_t takesRight = (color ? pawns >> 7 : pawns << 9) & NOTFILEA;
    uint64_t takesLeft = (color ? pawns >> 9 : pawns << 7) & NOTFILEH;
    uint64_t takes = takesLeft | takesRight;
    eval = eval+3*popcount(takes & pawns);
#endif*/

    return eval*(1-2*color);
}

extern evaluationResult result;

int32_t pawnEvaluation(const chessPosition* position, uint8_t* pawnColumnOccupancy, uint16_t phase) {
    uint32_t eval = 0;
    uint64_t whitePawns = position->pieceTables[white][pawn];
    uint64_t blackPawns = position->pieceTables[black][pawn];
    int16_t staticPawn = 0;

    pawnHashEntry entry;
    if (getPawnHashTableEntry(&entry, position->pawnHash)) {
        staticPawn = entry.eval;
        staticpawnhashhits++;
        pawnColumnOccupancy[0] = entry.pawnColumnOcc[0];
        pawnColumnOccupancy[1] = entry.pawnColumnOcc[1];

/*#ifdef DEBUG
        int16_t staticPawnReal = staticPawnEval(whitePawns, white, pawnColumnOccupancy, &evalPars->staticPawnParameters)+staticPawnEval(blackPawns, black,  pawnColumnOccupancy+1, &evalPars->staticPawnParameters);
        assert(staticPawnReal == staticPawn);
        assert(pawnColumnOccupancy[0] == entry.pawnColumnOcc[0]);
        assert(pawnColumnOccupancy[1] == entry.pawnColumnOcc[1]);
#endif*/
    } else {
        staticpawncalls++;
        staticPawn = staticPawnEvalComplete(position, pawnColumnOccupancy);
        setPawnHashEntry(staticPawn, pawnColumnOccupancy[0], pawnColumnOccupancy[1], position->pawnHash);
    }
    eval = eval+staticPawn;
    result.staticPawn = staticPawn;
    int32_t untapered = 0;
    int32_t passedPawns = passedPawnEval(&untapered, whitePawns, blackPawns, findLSB(position->pieceTables[black][king]), findLSB(position->pieceTables[white][king]), position->pieces[white], position->pieces[black]);
/*#ifdef EXPERIMENTAL
    int16_t passedPawnPhase = std::max(phase-10, 0);
#else*/

//#endif
    int16_t passedPawnPhase = std::max((int32_t) phase, 0);
    passedPawns = ((256-taperingValues[passedPawnPhase])*passedPawns)/256;
    eval = eval+passedPawns+untapered;
    result.passedPawn = passedPawns+untapered;

    return eval;
}
