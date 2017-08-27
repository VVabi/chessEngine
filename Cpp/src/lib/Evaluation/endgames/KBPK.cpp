/*
 * KBPK.cpp
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

int16_t KBPK_endgame(const chessPosition* position) {
    playerColor toWin;
    if (position->figureEval > 0) {
        toWin = white;
    } else {
        toWin = black;
    }

    uint16_t pawnfield   = findLSB(position->pieceTables[toWin][pawn]);
    uint16_t bishopfield = findLSB(position->pieceTables[toWin][bishop]);
    uint16_t winningKingField = findLSB(position->pieceTables[toWin][king]);
    uint16_t losingKingField  = findLSB(position->pieceTables[INVERTCOLOR(toWin)][king]);

    uint16_t distToPromotion = ROW(pawnfield);

    if (toWin == white) {
        distToPromotion = 7-ROW(pawnfield);
    }
    uint16_t promotionField;
    if (toWin == white) {
        promotionField = FILE(pawnfield)+56;
    } else {
        promotionField = FILE(pawnfield);
    }

    if (distBetweenFields(promotionField, losingKingField)+ (position->toMove == toWin? 1: 0) > distToPromotion+1) {
        return (1-2*toWin)*(600+40-10*distToPromotion);
    }


    int16_t eval = 450;

    if (FILE(pawnfield) == 0) {
        uint16_t cornerparity;
        int16_t bishopParity = (ROW(bishopfield)-FILE(bishopfield)) % 2;
        if (toWin == white) {
            cornerparity = 1;
        } else {
            cornerparity = 0;
        }

        if (cornerparity != bishopParity) {
            uint16_t cornerField;
            if (toWin == white) {
                cornerField = 56;
            } else {
                cornerField = 0;
            }

            if (distBetweenFields(losingKingField, cornerField) <= distBetweenFields(winningKingField, cornerField)) { //TODO: this is still pretty crude
                return 0;
            }
        }
    }

    if (FILE(pawnfield) == 7) {
        uint16_t cornerparity;
        int16_t bishopParity = (ROW(bishopfield)-FILE(bishopfield)) % 2;
        if (toWin == white) {
            cornerparity = 0;
        } else {
            cornerparity = 1;
        }

        if (cornerparity != bishopParity) {
            uint16_t cornerField;
            if (toWin == white) {
                cornerField = 63;
            } else {
                cornerField = 7;
            }

            if (distBetweenFields(losingKingField, cornerField) <= distBetweenFields(winningKingField, cornerField)) {  //TODO: this is still pretty crude
                return 0;
            }
        }
    }
    return (1-2*toWin)*(eval+40-10*distToPromotion);
}
