/*
 * KPK.cpp
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



int16_t KPK_endgame(const chessPosition* position) { //currently only rook pawn handling. TODO: extend!
    playerColor toWin;
    if (position->figureEval > 0) {
        toWin = white;
    } else {
        toWin = black;
    }

    uint16_t pawnfield   = findLSB(position->pieceTables[toWin][pawn]);
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

    if (distBetweenFields(promotionField, losingKingField)+ (position->toMove == toWin? 1: 0) > distToPromotion+1) { //TODO: this is doubled
        return COLORSIGN(toWin)*(400+40-10*distToPromotion);
    }

    if (distToPromotion > 1) {
        if (FILE(pawnfield) == 7) {
            uint64_t forbidden_mask;
            if (toWin == black) {
                forbidden_mask = H1 | G1 | F1 | H2 | G2 | F2 | G3 | H3;
            } else {
                forbidden_mask = H8 | G8 | F8 | H7 | G7 | F7 | G6 | H6;
            }

            if (BIT64(losingKingField) & forbidden_mask) {
                return 0;
            }
        }

        if (FILE(pawnfield) == 0) {
            uint64_t forbidden_mask;
            if (toWin == black) {
                forbidden_mask = A1 | B1 | C1 | A2 | B2 | C2 | B3 | A3;
            } else {
                forbidden_mask = A8 | B8 | C8 | A7 | B7 | C7 | B6 | A6;
            }

            if (BIT64(losingKingField) & forbidden_mask) {
                return 0;
            }
        }
    }
    if ((FILE(pawnfield) > 0) && (FILE(pawnfield) < 7)) {
        if (toWin == white) {
            if (pawnfield+16 == winningKingField) {
                return COLORSIGN(toWin)*(300+70-10*distToPromotion);
            }
        }

        if (toWin == black) {
            if (pawnfield-16 == winningKingField) {
                return COLORSIGN(toWin)*(300+70-10*distToPromotion);
            }
        }
    }
    return COLORSIGN(toWin)*(100+70-10*distToPromotion);
}
