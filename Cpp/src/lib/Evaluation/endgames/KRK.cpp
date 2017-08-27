/*
 * KRK.cpp
 *
 *  Created on: Aug 27, 2017
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

int16_t KRK_endgame(const chessPosition* position) {
    int16_t eval = 700;//base evaluation for rook endgame, 500 rook value+200 to encourage simplifying to here


    playerColor toWin;
    if (position->figureEval > 0) {
        toWin = white;
    } else {
        toWin = black;
    }

    uint16_t winningKingField = findLSB(position->pieceTables[toWin][king]);
    uint16_t losingKingField  = findLSB(position->pieceTables[INVERTCOLOR(toWin)][king]);


    eval = eval-getEndgameGamePSQentry(king, INVERTCOLOR(toWin), losingKingField);
    eval = eval+40-10*distBetweenFields(winningKingField, losingKingField);

    return (1-2*toWin)*eval;
}

