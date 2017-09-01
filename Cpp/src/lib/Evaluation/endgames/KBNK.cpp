/*
 * KBNK.cpp
 *
 *  Created on: Aug 29, 2017
 *      Author: vabi
 */




#include <lib/basics.hpp>
#include <lib/Evaluation/PSQ.hpp>
#include <lib/bitfiddling.h>
#include <algorithm>

static uint16_t distBetweenFields(uint16_t a, uint16_t b) {
    uint16_t fileA = FILE(a);
    uint16_t rowA  = ROW(a);
    uint16_t fileB = FILE(b);
    uint16_t rowB  = ROW(b);

    uint16_t rowDiff = (rowA > rowB? rowA-rowB: rowB-rowA);

    uint16_t fileDiff = (fileA > fileB? fileA-fileB : fileB-fileA);

    return std::max(rowDiff, fileDiff);
}

int16_t KBNK_endgame(const chessPosition* position) {
    //TODO: this is too simple - it works, but its pretty much unwatchable
    int16_t eval = 700;//base evaluation for bishop/knight endgame
    playerColor toWin;
    if (position->figureEval > 0) {
        toWin = white;
    } else {
        toWin = black;
    }

    uint16_t winningKingField = findLSB(position->pieceTables[toWin][king]);
    uint16_t losingKingField  = findLSB(position->pieceTables[INVERTCOLOR(toWin)][king]);
    uint16_t bishopField      = findLSB(position->pieceTables[toWin][bishop]);

    uint16_t parity = FILE(bishopField)+ROW(bishopField);

    uint16_t good_corners[2];

    if ((parity & 1)) {
        good_corners[0] = 56;
        good_corners[1] = 7;
    } else {
        good_corners[0] = 0;
        good_corners[1] = 63;
    }

    uint16_t distToMateField = std::min(distBetweenFields(losingKingField, good_corners[0]), distBetweenFields(losingKingField, good_corners[1]));

    uint16_t kingDist = distBetweenFields(winningKingField, losingKingField);

    eval = eval+100-25*distToMateField+100-10*kingDist;

    return COLORSIGN(toWin)*eval;
}
