/*
 * king_endgame_position.cpp
 *
 *  Created on: Dec 23, 2018
 *      Author: vabi
 */

#include <lib/basics.hpp>
#include <lib/bitfiddling.h>
#include <lib/Evaluation/evaluation.hpp>
#include <algorithm>

static inline uint16_t distBetweenFields(uint16_t a, uint16_t b) {
    uint16_t fileA = FILE(a);
    uint16_t rowA  = ROW(a);
    uint16_t fileB = FILE(b);
    uint16_t rowB  = ROW(b);

    uint16_t rowDiff = (rowA > rowB? rowA-rowB: rowB-rowA);

    uint16_t fileDiff = (fileA > fileB? fileA-fileB : fileB-fileA);

    return std::max(rowDiff, fileDiff);
}

static inline uint16_t calcClosestDistance(uint16_t field, uint64_t targetPositions) {
    uint16_t dist = 255;
    while (targetPositions) {
        uint16_t next = popLSB(targetPositions);
        dist = std::min(dist, distBetweenFields(field, next));
    }

    return dist;
}

static int16_t evalScores[] = {10, 8, 5, 2, 0, -5, -10, -15};

EvalComponentResult kingEndgamePosition(const chessPosition* position,
        const evalParameters* par __attribute__((unused)),
        EvalMemory* evalMemory __attribute__((unused))) {
    EvalComponentResult result;
    uint64_t wpawns         = position->pieceTables[white][pawn];
    uint16_t wkingField     = findLSB(position->pieceTables[white][king]);
    uint64_t bpawns         = position->pieceTables[black][pawn];
    uint16_t bkingField     = findLSB(position->pieceTables[black][king]);

    uint16_t minDist = calcClosestDistance(wkingField, wpawns);
    if (minDist < 8) {
        result.endgame += evalScores[minDist];
    }

    minDist = calcClosestDistance(wkingField, bpawns);
    if (minDist < 8) {
        result.endgame += evalScores[minDist];
    }

    minDist = calcClosestDistance(bkingField, wpawns);
    if (minDist < 8) {
        result.endgame -= evalScores[minDist];
    }

    minDist = calcClosestDistance(bkingField, bpawns);
    if (minDist < 8) {
        result.endgame -= evalScores[minDist];
    }
    return result;
}
