/*
 * kingsafety.cpp
 *
 *  Created on: Oct 4, 2016
 *      Author: vabi
 */



#include "lib/basics.hpp"
#include "lib/Evaluation/evaluation.hpp"
#include "lib/bitfiddling.h"
#include "lib/Defines/boardParts.hpp"
#include "lib/Attacks/attacks.hpp"
#include "lib/moveGeneration/nonSliderMoveTables.hpp"

static int16_t kingSafetySinglePlayer(const chessPosition* position, const uint8_t* pawnColumnOccupancy,
        playerColor playingSide, const AttackTable* opponentAttackTable, const kingSafetyEvalParameters* par) {
    int16_t ret = 0;
    //pawn shield
    uint16_t kingField = findLSB(position->pieceTables[playingSide][king]);
    uint16_t kingFile = FILE(kingField);
    if (!((1 << kingFile) & pawnColumnOccupancy[playingSide])) { // no pawn in front of king. TODO: check for pawn really in FRONT of king
        ret = ret+par->selfopenfiletoking;
    }

    if (!((1 << kingFile) & pawnColumnOccupancy[1-playingSide])) { // no opponent pawn
        ret = ret+par->opponentopenfiletoking;
    }

    if (kingFile > 0) {
        if (!((1 << (kingFile-1)) & pawnColumnOccupancy[playingSide])) { // no pawn in front of king. TODO: check for pawn really in FRONT of king
                ret = ret+par->selfopenfilenexttoking;
        }
        if (!((1 << (kingFile-1)) & pawnColumnOccupancy[1-playingSide])) { // no opponent pawn
            ret = ret+par->opponentopenfilenexttoking;
        }
    }

    if (kingFile < 7) {
        if (!((1 << (kingFile+1)) & pawnColumnOccupancy[playingSide])) { // no pawn in front of king. TODO: check for pawn really in FRONT of king
            ret = ret+par->selfopenfilenexttoking;
        }
        if (!((1 << (kingFile+1)) & pawnColumnOccupancy[1-playingSide])) { // no opponent pawn
            ret = ret+par->opponentopenfilenexttoking;
        }
    }


    uint64_t kingmoves = getKingMoves(kingField);
    kingmoves = kingmoves | BIT64(kingField);

    uint16_t kingAttackScore = 0;

    for (uint16_t pieceType = 0; pieceType < 5; pieceType++) {
        uint64_t attacks = opponentAttackTable->attackTables[pieceType] & kingmoves;
        kingAttackScore = kingAttackScore+popcount(attacks)*par->attackScores[pieceType];
    }


    if (kingAttackScore >= 70) {
        kingAttackScore = 69;
    }

    ret = ret-par->attacksCloseToKingEvals[kingAttackScore];
    return (1-2*playingSide)*ret;
}


EvalComponentResult kingSafety(const chessPosition* position, const evalParameters* par, const AttackTable* attackTables) {
    uint8_t pawnColumnOccupancy[2];

    for (uint8_t color=0; color < 2; color++) {
        uint64_t pawns = position->pieceTables[color][pawn];
        pawns = northFill(pawns);
        pawns = southFill(pawns);
        pawnColumnOccupancy[color] = (uint8_t) (pawns & 0xFF);
    }

    int16_t whiteSafety =  kingSafetySinglePlayer(position, pawnColumnOccupancy, white, &attackTables[black], &par->kingSafetyParameters);
    int16_t blackSafety =  kingSafetySinglePlayer(position, pawnColumnOccupancy, black, &attackTables[white], &par->kingSafetyParameters);
    EvalComponentResult ret;
#ifdef EXPERIMENTAL
    ret.early_game = (4*(whiteSafety+blackSafety))/10;
#else
    ret.early_game = whiteSafety+blackSafety;
#endif

    return ret;
}
