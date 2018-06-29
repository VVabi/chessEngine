/*
 * pawnEvaluation.cpp
 *
 *  Created on: Sep 25, 2016
 *      Author: vabi
 */

#include <algorithm>
#include <lib/basics.hpp>
#include "lib/Evaluation/evaluation.hpp"
#include <lib/bitfiddling.h>
#include <lib/Defines/boardParts.hpp>
#include <parameters/parameters.hpp>
#include <hashTables/hashTables.hpp>
#include <lib/Defines/passedPawns.hpp>
#include <lib/Evaluation/tapering.hpp>

uint64_t files[] = {FILEA, FILEB, FILEC, FILED, FILEE, FILEF, FILEG, FILEH};

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


int16_t passedPawnEval(int32_t* untaperedEval, uint64_t whitePawns, uint64_t blackPawns, uint16_t blackKing, uint16_t whiteKing, uint64_t whitePieces, uint64_t blackPieces) {
    //TODO: remove code duplication. Generally this code sucks - too many white/black diffs...
    int32_t eval = 0;
    uint64_t whitePawnBuffer = whitePawns;

    while (whitePawnBuffer) {
        uint16_t field = popLSB(whitePawnBuffer);
        if ((getPassedPawnMask(white, field) & blackPawns) == 0) {
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
            eval = eval-kingToPromotionFieldDistance[distToPromotion][kingDist];
        }
    }
    uint64_t blackPawnBuffer = blackPawns;
    while (blackPawnBuffer) {
        uint16_t field = popLSB(blackPawnBuffer);
        if ((getPassedPawnMask(black, field) & whitePawns) == 0) {
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

    return eval*(1-2*color);
}


int16_t staticPawnEvaluation(const chessPosition* position) {
    uint32_t eval = 0;
    int16_t staticPawn = 0;

    pawnHashEntry entry;
    if (getPawnHashTableEntry(&entry, position->pawnHash)) {
        staticPawn = entry.eval;
        staticpawnhashhits++;
        //TODO add debug code here to verify the hash is correct!
    } else {
        staticpawncalls++;
        staticPawn = staticPawnEvalComplete(position);
        setPawnHashEntry(staticPawn, position->pawnHash);
    }
    eval = eval+staticPawn;

    return eval;
}
