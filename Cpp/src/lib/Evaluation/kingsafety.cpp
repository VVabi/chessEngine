/*
 * kingsafety.cpp
 *
 *  Created on: Oct 4, 2016
 *      Author: vabi
 */



#include <lib/basics.hpp>
#include "evaluation.hpp"
#include <lib/bitfiddling.h>
#include <lib/Defines/boardParts.hpp>
#include <lib/Attacks/attacks.hpp>
#include <lib/moveGeneration/nonSliderMoveTables.hpp>
#include <lib/moveGeneration/moveGenerationInternals.hpp>

/*#ifdef EXPERIMENTAL
int32_t attacksCloseToKingEvals[] =
{ 0, 0, 1, 2, 3, 4, 5, 6, 7, 9,
 11, 13, 16, 20, 24, 28, 32, 36, 40, 44,
 48, 52, 56, 60, 64, 68, 72, 76, 80, 85,
 91, 97, 103, 109, 115, 121, 127, 134, 142,
 152, 163, 175, 188, 202, 217, 232, 246, 260,
 273, 286, 299, 312, 324, 336, 348, 360, 372, 384,
 392, 400, 405, 410, 410, 410, 410, 410, 410, 410
};

#else*/
int32_t attacksCloseToKingEvals[] =
{ 0, 0, 1, 2, 3, 5, 8, 10, 13, 20,
 23, 26, 29, 33, 37, 41, 45, 51, 57, 63,
 69, 75, 82, 89, 96, 103, 110, 118, 127, 136,
 145, 154, 163, 172, 181, 190, 199, 208, 217, 216,
 226, 236, 246, 256, 266, 276, 286, 296, 306, 316,
 326, 336, 346, 356, 366, 376, 386, 395, 404, 413,
 421, 429, 437, 441, 450, 450, 450, 450, 450, 450
};
//#endif

/*#ifdef EXPERIMENTAL
int32_t attackScores[6][5] = { {0, 0, 0, 0, 0},
                               {1, 3, 3, 4, 6},
                               {2, 4, 4, 5, 9},
                               {2, 5, 5, 7, 11},
                               {2, 6, 6, 8, 12},
                               {0, 0, 0, 0, 0}
};
#else*/
int32_t attackScores[] = {1, 3, 3, 4, 7};
//#endif

#ifdef EXPERIMENTAL
static int16_t shelterscores[80] = {
          0, 10, 12, 13, 15, 17, 19, 21, 23, 25,
         27, 29, 31, 33, 35, 37, 39, 41, 43, 45,
         47, 49, 51, 54, 57, 60, 63, 66, 68, 70,
         72, 74, 76, 78, 80, 82, 84, 86, 88, 90,
         91, 92, 93, 94, 95, 96, 97, 98, 99, 100,
         100,100,100,100,100,100,100,100,100,100,
         100,100,100,100,100,100,100,100,100,100,
         100,100,100,100,100,100,100,100,100,100,
};
#endif

//int32_t attacksCloseToKingEvals[] = {0, 10, 20, 40, 80, 150, 230, 350, 400, 500, 600};
#ifdef EXPERIMENTAL
static uint64_t files[] = {FILEA, FILEB, FILEC, FILED, FILEE, FILEF, FILEG, FILEH};
#endif



#ifdef EXPERIMENTAL
static int16_t pawnShelterSingleFile(playerColor playingSide, uint64_t pawnsOnFile) {
    uint16_t distance;
    if (playingSide == white) {
        distance = ROW(findLSB(pawnsOnFile))-1;
    } else {
        distance = 6-ROW(findMSB(pawnsOnFile));
    }

    if (pawnsOnFile == 0) {
        distance = 8;
    }

    return distance;
}

static int16_t pawnShelter(const chessPosition* position, playerColor playingSide) {
    uint16_t kingField = findLSB(position->pieceTables[playingSide][king]);
    uint16_t kingFile = FILE(kingField);
    uint16_t score = 0;
    uint64_t pawns    = position->pieceTables[playingSide][pawn];
    uint64_t oppPawns = position->pieceTables[INVERTCOLOR(playingSide)][pawn];
    uint64_t kingPawn = (pawns & files[kingFile]);

    uint16_t distance = pawnShelterSingleFile(playingSide, kingPawn);
    if (!(files[kingFile] & oppPawns)) {
        score = score+2;
        if (distance == 8) {
            score += 2*distance+8;
        }
    } else {
        score += distance;
    }

    if (kingFile > 0) {
        uint16_t neighborFile = kingFile-1;
        uint16_t distance = pawnShelterSingleFile(playingSide, files[neighborFile] & pawns);
           if (!(files[neighborFile] & oppPawns)) {
               score = score+2;
               if (distance == 8) {
                   score += 2*distance+8;
               }
           } else {
               score += distance;
           }
    }

    if (kingFile < 7) {
        uint16_t neighborFile = kingFile+1;
        uint16_t distance = pawnShelterSingleFile(playingSide, files[neighborFile] & pawns);
           if (!(files[neighborFile] & oppPawns)) {
               score = score+2;
               if (distance == 8) {
                   score += 2*distance+8;
               }
           } else {
               score += distance;
           }
    }

    assert(score < 80);
    return -shelterscores[score];
}
#endif


#ifdef EXPERIMENTAL
void outputUint64(uint64_t num);

static uint16_t getFreeKingFields(uint16_t kingField, uint64_t occ) {
    uint64_t modOcc = occ | EASTONE(BIT64(kingField)) | WESTONE((BIT64(kingField)));
    uint64_t mask = getPotentialRookMoves(kingField, modOcc);
    mask |= getPotentialBishopMoves(kingField, occ);
    return popcount(mask);
}
#endif

static int32_t kingSafetySinglePlayer(const chessPosition* position, const uint8_t* pawnColumnOccupancy,
        playerColor playingSide, const AttackTable* opponentAttackTable, const kingSafetyEvalParameters* par) {
    int32_t ret = 0;
    //pawn shield

    uint16_t kingField = findLSB(position->pieceTables[playingSide][king]);
#ifdef EXPERIMENTAL
#define SELF 768
#define NEIGHBOR 384
#define REMOTE   256
    ret = ret-(SELF*getFreeKingFields(kingField, position->pieceTables[white][pawn] | position->pieceTables[black][pawn]))/256;
    if (FILE(kingField) > 0){
        ret = ret-(NEIGHBOR*getFreeKingFields(kingField-1, position->pieceTables[white][pawn] | position->pieceTables[black][pawn]))/256;
    } else {
        ret = ret-(REMOTE*getFreeKingFields(kingField+2, position->pieceTables[white][pawn] | position->pieceTables[black][pawn]))/256;
    }
    if (FILE(kingField) < 7){
        ret = ret-(NEIGHBOR*getFreeKingFields(kingField+1, position->pieceTables[white][pawn] | position->pieceTables[black][pawn]))/256;
    } else {
        ret = ret-(REMOTE*getFreeKingFields(kingField-2, position->pieceTables[white][pawn] | position->pieceTables[black][pawn]))/256;
    }
#else
//#ifndef EXPERIMENTAL
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
#endif
/*#else
    ret = ret+pawnShelter(position, playingSide);

#endif*/
    /*uint64_t relevant_files = files[FILE(kingField)];
    if (kingFile > 0) {
        relevant_files = relevant_files | files[FILE(kingField-1)];
    }

    if (kingFile < 7) {
        relevant_files = relevant_files | files[FILE(kingField+1)];
    }*/


    /*uint64_t opponentPieces = position->pieces[1-playingSide];
    uint64_t opponentQueens = position->pieceTables[1-playingSide][queen];*/

    uint64_t kingmoves = getKingMoves(kingField);
    kingmoves = kingmoves | BIT64(kingField);
/*#ifdef EXPERIMENTAL
    if (playingSide == white) {
        kingmoves = kingmoves | (kingmoves << 8);
    } else {
        kingmoves = kingmoves | (kingmoves >> 8);
    }
#endif*/
    uint16_t kingAttackScore = 0;

    for (uint16_t pieceType = 0; pieceType < 5; pieceType++) {
        uint64_t attacks = opponentAttackTable->attackTables[pieceType] & kingmoves;

/*#ifdef EXPERIMENTAL
        uint16_t numAttacks = popcount(attacks);
        if (numAttacks > 4) {
            numAttacks = 4;
        }
        kingAttackScore = kingAttackScore+attackScores[numAttacks][pieceType];
#else*/
    kingAttackScore = kingAttackScore+popcount(attacks)*attackScores[pieceType];
//#endif
    }


    if (kingAttackScore >= 70) {
        kingAttackScore = 69;
    }
/*#ifdef EXPERIMENTAL
    ret = ret-(3*attacksCloseToKingEvals[kingAttackScore])/2;
#else*/
    ret = ret-attacksCloseToKingEvals[kingAttackScore];
//#endif

    /*uint64_t attacks = opponentAttackTable->completeAttackTable & kingmoves;

    ret = ret-attacksCloseToKingEvals[popcount(attacks)];*/



    return (1-2*playingSide)*ret;
}


int32_t kingSafety(const chessPosition* position, const uint8_t* pawnColumnOccupancy,
    const AttackTable* whiteAttackTable, const AttackTable* blackAttackTable, const kingSafetyEvalParameters* par) {
    int32_t whiteSafety =  kingSafetySinglePlayer(position, pawnColumnOccupancy, white, blackAttackTable, par);
    int32_t blackSafety =  kingSafetySinglePlayer(position, pawnColumnOccupancy, black, whiteAttackTable, par);
    int32_t ret =  whiteSafety+blackSafety;
    return ret;
}
