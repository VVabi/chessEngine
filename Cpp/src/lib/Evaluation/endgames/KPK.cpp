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

static uint64_t white_key_squares[] = {
			0,0,0,0,0,0,0,0,
			0, A4 | B4 | C4, B4 | C4 | D4, C4 | D4 | E4, D4 | E4 | F4, E4 | F4 | G4, F4 | G4 | H4, 0,
			0, A5 | B5 | C5, B5 | C5 | D5, C5 | D5 | E5, D5 | E5 | F5, E5 | F5 | G5, F5 | G5 | H5, 0,
			0, A6 | B6 | C6, B6 | C6 | D6, C6 | D6 | E6, D6 | E6 | F6, E6 | F6 | G6, F6 | G6 | H6, 0,
			0, A6 | B6 | C6 | A7 | B7 | C7, B6 | C6 | D6 | B7 | C7 | D7, C6 | D6 | E6 | C7 | D7 | E7, D6 | E6 | F6 | D7 | E7 | F7, E6 | F6 | G6 | E7 | F7 | G7, F6 | G6 | H6 | F7 | G7 | H7, 0,
			B7 | B8, A7 | B7 | C7 | A8 | B8 | C8, B7 | C7 | D7 | B8 | C8 | D8, C7 | D7 | E7 | C8 | D8 | E8, D7 | E7 | F7 | D8 | E8 | F8, E7 | F7 | G7 | E8 | F8 | G8, F7 | G7 | H7 | F8 | G8 | H8, G7 | G8,
			B7 | B8, A7 | B7 | C7 | A8 | B8 | C8, B7 | C7 | D7 | B8 | C8 | D8, C7 | D7 | E7 | C8 | D8 | E8, D7 | E7 | F7 | D8 | E8 | F8, E7 | F7 | G7 | E8 | F8 | G8, F7 | G7 | H7 | F8 | G8 | H8, G7 | G8,
			0,0,0,0,0,0,0,0
};

static uint64_t black_key_squares[] = {
			0,0,0,0,0,0,0,0,
			B2 | B1, A2 | B2 | C2 | A1 | B1 | C1, B2 | C2 | D2 | B1 | C1 | D1, C2 | D2 | E2 | C1 | D1 | E1, D2 | E2 | F2 | D1 | E1 | F1, E2 | F2 | G2 | E1 | F1 | G1, F2 | G2 | H2 | F1 | G1 | H1, G2 | G1,
			B2 | B1, A2 | B2 | C2 | A1 | B1 | C1, B2 | C2 | D2 | B1 | C1 | D1, C2 | D2 | E2 | C1 | D1 | E1, D2 | E2 | F2 | D1 | E1 | F1, E2 | F2 | G2 | E1 | F1 | G1, F2 | G2 | H2 | F1 | G1 | H1, G2 | G1,
			0, A3 | B3 | C3 | A2 | B2 | C2, B3 | C3 | D3 | B2 | C2 | D2, C3 | D3 | E3 | C2 | D2 | E2, D3 | E3 | F3 | D2 | E2 | F2, E3 | F3 | G3 | E2 | F2 | G2, F3 | G3 | H3 | F2 | G2 | H2, 0,
			0, A3 | B3 | C3, B3 | C3 | D3, C3 | D3 | E3, D3 | E3 | F3, E3 | F3 | G3, F3 | G3 | H3, 0,
			0, A4 | B4 | C4, B4 | C4 | D4, C4 | D4 | E4, D4 | E4 | F4, E4 | F4 | G4, F4 | G4 | H4, 0,
			0, A5 | B5 | C5, B5 | C5 | D5, C5 | D5 | E5, D5 | E5 | F5, E5 | F5 | G5, F5 | G5 | H5, 0,
			0,0,0,0,0,0,0,0
};


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
                forbidden_mask = H1 | G1  | H2 | G2 | F2 | H3;
            } else {
                forbidden_mask = H8 | G8  | H7 | G7 | F7 | H6;
            }

            if (BIT64(losingKingField) & forbidden_mask) {
                return 0;
            }
        }

        if (FILE(pawnfield) == 0) {
            uint64_t forbidden_mask;
            if (toWin == black) {
                forbidden_mask = A1 | B1  | A2 | B2 | C2 | A3;
            } else {
                forbidden_mask = A8 | B8  | A7 | B7 | C7 | A6;
            }

            if (BIT64(losingKingField) & forbidden_mask) {
                return 0;
            }
        }
    }
    //if ((FILE(pawnfield) > 0) && (FILE(pawnfield) < 7)) { //TODO: this is very discontinuous
	if (toWin == white) {
		if (pawnfield < 48) {
			if (pawnfield == losingKingField-8 || pawnfield == losingKingField-16) {
				return 0;
			}
		}

		if (white_key_squares[pawnfield] & BIT64(winningKingField)) {
			return COLORSIGN(toWin)*(300+70-10*distToPromotion);
		}
	}

	if (toWin == black) {
		if (pawnfield > 16) {
			if (pawnfield == losingKingField+8 || pawnfield == losingKingField+16) {
				return 0;
			}
		}

		if (black_key_squares[pawnfield] & BIT64(winningKingField)) {
			return COLORSIGN(toWin)*(300+70-10*distToPromotion);
		}
	}
    //}
    return COLORSIGN(toWin)*(100+70-10*distToPromotion);
}
