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

//uint64_t files[] = { FILEA, FILEB, FILEC, FILED, FILEE, FILEF, FILEG, FILEH };

static uint16_t distBetweenFields(uint16_t a, uint16_t b) {
    uint16_t fileA = FILE(a);
    uint16_t rowA = ROW(a);
    uint16_t fileB = FILE(b);
    uint16_t rowB = ROW(b);

    uint16_t rowDiff = (rowA > rowB ? rowA - rowB : rowB - rowA);

    uint16_t fileDiff = (fileA > fileB ? fileA - fileB : fileB - fileA);

    return std::max(rowDiff, fileDiff);
}

uint64_t staticpawncalls = 0;
uint64_t staticpawnhashhits = 0;

#define BLOCKEDDIVISOR 2

EvalComponentResult passedPawnEval(const chessPosition* position,
        const evalParameters* par __attribute__((unused)),
        EvalMemory* evalMemory) {
    //TODO: remove code duplication. Generally this code sucks - too many white/black diffs...
    int32_t eval = 0;
    uint64_t whitePawns = position->pieceTables[white][pawn];
    uint64_t blackPawns = position->pieceTables[black][pawn];
    uint64_t whitePawnBuffer = whitePawns;

    uint16_t blackKing = findLSB(position->pieceTables[black][king]);
    uint16_t whiteKing = findLSB(position->pieceTables[white][king]);

    int16_t untaperedEval = 0;
    uint64_t wPassedPawns = 0;
    while (whitePawnBuffer) {
        uint16_t field = popLSB(whitePawnBuffer);
        if ((getPassedPawnMask(white, field) & blackPawns) == 0) {
            wPassedPawns = wPassedPawns | BIT64(field);
            uint16_t promotionField = FILE(field) + 56;
            uint16_t distToPromotion = 7 - ROW(field);
            uint16_t kingDist = distBetweenFields(promotionField, blackKing);
            bool blocked = position->pieces[black] & BIT64(field + 8);
            if (distToPromotion <= 2) {
                untaperedEval = untaperedEval + par->passedPawnParameters.passedPawnEvalValues[white][field];
                if (blocked) {
                    untaperedEval = untaperedEval - par->passedPawnParameters.passedPawnEvalValues[white][field]/ BLOCKEDDIVISOR;
                }
            } else {
                eval = eval + par->passedPawnParameters.passedPawnEvalValues[white][field];
                if (blocked) {
                    eval = eval - par->passedPawnParameters.passedPawnEvalValues[white][field]/ BLOCKEDDIVISOR;
                }
            }
            //TODO: I think we should add more static knowledge about when a pawn is passed
            if ((distToPromotion == 1) && (position->toMove == white) && (((evalMemory->attackTables[black].completeAttackTable & BIT64(promotionField)) == 0))) {
                untaperedEval += 300;
            }
            eval = eval - par->passedPawnParameters.kingToPromotionFieldDistance[distToPromotion][kingDist];
        }
    }
    uint64_t blackPawnBuffer = blackPawns;
    uint64_t bPassedPawns = 0;
    while (blackPawnBuffer) {
        uint16_t field = popLSB(blackPawnBuffer);
        if ((getPassedPawnMask(black, field) & whitePawns) == 0) {
            bPassedPawns = bPassedPawns | BIT64(field);
            uint16_t promotionField = FILE(field);
            uint16_t distToPromotion = ROW(field);
            uint16_t kingDist = distBetweenFields(promotionField, whiteKing);
            bool blocked = position->pieces[white] & BIT64(field - 8);
            if (distToPromotion <= 2) {
                untaperedEval = untaperedEval - par->passedPawnParameters.passedPawnEvalValues[black][field];
                if (blocked) {
                    untaperedEval = untaperedEval + par->passedPawnParameters.passedPawnEvalValues[black][field]/ BLOCKEDDIVISOR;
                }
            } else {
                eval = eval - par->passedPawnParameters.passedPawnEvalValues[black][field];
                if (blocked) {
                    eval = eval + par->passedPawnParameters.passedPawnEvalValues[black][field]/ BLOCKEDDIVISOR;
                }
            }
            eval = eval + par->passedPawnParameters.kingToPromotionFieldDistance[distToPromotion][kingDist];

            //TODO: I think we should add more static knowledge about when a pawn is passed
            if ((distToPromotion == 1) && (position->toMove == black) && (((evalMemory->attackTables[white].completeAttackTable & BIT64(promotionField)) == 0))) {
                untaperedEval -= 300;
            }
        }
    }
    evalMemory->passedPawns[white] = wPassedPawns;
    evalMemory->passedPawns[black] = bPassedPawns;

    EvalComponentResult result;
    result.common = untaperedEval;
    result.endgame = eval;

    return result;
}
