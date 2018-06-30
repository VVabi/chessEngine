/*
 * KBBK.cpp
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

int16_t KBBK_endgame(const chessPosition* position) {
    int16_t eval = 700;//base evaluation for bishop endgame
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

    uint16_t parity = (position->toMove == toWin ? 0 : 1);
    uint16_t movesAtBorder = 0;
    for (int16_t cnt = position->madeMoves.length-1-parity; cnt > 0; cnt = cnt-2) {
        chessMove mv = position->madeMoves[cnt];
        if (BIT64(mv.targetField) & (~BOUNDARY)) {
            break;
        }
        movesAtBorder++;
    }

    eval = eval+10*movesAtBorder;

    eval = eval-position->data.fiftyMoveRuleCounter;
    return (1-2*toWin)*eval;
}
