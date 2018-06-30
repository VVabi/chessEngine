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

    eval = eval-position->data.fiftyMoveRuleCounter;


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

    /*uint16_t rookfield = findLSB(position->pieceTables[toWin][rook]);

    uint16_t rookfile  = FILE(rookfield);
    uint16_t losingKingFile = FILE(losingKingField);
    uint16_t winningKingFile = FILE(winningKingField);
    uint16_t rookrow  = ROW(rookfield);
    uint16_t losingKingrow = ROW(losingKingField);
    uint16_t winningKingrow = ROW(winningKingField);


    if ((losingKingrow == 7) || (losingKingrow == 0) || (losingKingFile == 0) || (losingKingrow == 7)) {
        eval = eval+100;
        if ((losingKingFile+1 == rookfile) && (rookfile+1 == winningKingFile)) {
            eval = eval+15;
        }

        if ((losingKingFile-1 == rookfile) && (rookfile-1 == winningKingFile)) {
            eval = eval+15;
        }

        if ((losingKingrow+1 == rookrow) && (rookrow+1 == winningKingrow)) {
             eval = eval+15;
        }

        if ((losingKingrow-1 == rookrow) && (rookrow-1 == winningKingrow)) {
             eval = eval+15;
        }


        eval = eval + 5*distBetweenFields(rookfield, losingKingField);
    }*/

    return (1-2*toWin)*eval;
}

