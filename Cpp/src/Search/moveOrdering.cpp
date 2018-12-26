/*
 * moveOrdering.cpp
 *
 *  Created on: Sep 22, 2016
 *      Author: vabi
 */

#include <DataTypes/vdt_vector.hpp>
#include <lib/basics.hpp>
#include <lib/Defines/knightmoves.hpp>
#include <lib/Defines/kingMoves.hpp>
#include <lib/Defines/boardParts.hpp>
#include <lib/bitfiddling.h>
#include <algorithm>
#include <lib/Attacks/attacks.hpp>
#include <hashTables/hashTables.hpp>
#include <lib/moveMaking/moveMaking.hpp>
#include <parameters/parameters.hpp>
#include <cmath>
#include <userInterface/UIlayer.hpp>
#include <Search/search.hpp>
#include <Search/history.hpp>
#include <Search/killerMoves.hpp>
#include <lib/Evaluation/PSQ.hpp>

#define WHITEKINGCASTLECHESSFIELDS ((1ULL << 4) | (1ULL << 5) | (1ULL << 6))
#define WHITEQUEENCASTLECHESSFIELDS ((1ULL << 4) | (1ULL << 3) | (1ULL << 2))
#define BLACKKINGCASTLECHESSFIELDS ((1ULL << 60) | (1ULL << 61) | (1ULL << 62))
#define BLACKQUEENCASTLECHESSFIELDS ((1ULL << 60) | (1ULL << 59) | (1ULL << 58))

uint64_t kingBlockers[] = {WHITEKINGCASTLECHESSFIELDS, BLACKKINGCASTLECHESSFIELDS};
uint64_t queenBlockers[] = {WHITEQUEENCASTLECHESSFIELDS, BLACKQUEENCASTLECHESSFIELDS};

int16_t captureEvals[6][7] = {
        {20,    130,    130,    390,    900,    10000,  0},
        {0,     50,     50,     220,    700,    10000,  0},
        {0,     50,     50,     220,    700,    10000,  0},
        {-20,   40,     40,     210,    400,    10000,  0},
        {-70,   30,     30,     160,    400,    10000,  0},
        {0,     200,    200,    300,    700,    10000,  0},
};

static void calcCaptureSortEval(chessPosition* position, chessMove* mv, uint16_t hashedMove) {
    int16_t sortEval = SEE(position, mv);
    /*int16_t sortEval = 0;
    if (((uint16_t) mv->type) < 6) {
        sortEval = sortEval+captureEvals[mv->type][mv->captureType];
    }*/

    /*if (position->madeMoves.length > 0) {
        chessMove previousMove = position->madeMoves[position->madeMoves.length-1];
        if (previousMove.targetField == mv->sourceField) { //recapture is usually good
            sortEval = sortEval + 200;
        }
    }*/

    if (((mv->sourceField) | (mv->targetField << 8)) == hashedMove) {
        sortEval = sortEval+2000;
    }
    mv->sortEval = sortEval;
}

#define ILLEGAL -20000

static inline void calcSortEval(chessPosition* position, chessMove* mv, bool isInCheck, AttackTable* opponentAttackTable, AttackTable* ownAttackTable, uint16_t hashedMove, uint16_t killerA, uint16_t killerB, uint16_t refutationTarget, const evalParameters* evalPars) {
    //this function is completely ridiculously expensive. Whats going on??
    //TODO: add bonus for pawn pushes to 6th/7th row!
    /*if (isInCheck) {   //for some reason I don't fully understand, adding this here slows and changes the search?? Maybe because these moves then are not history-reduced? At least possible.
        uint16_t kingField = findLSB(position->pieceTables[position->toMove][king]);

        uint64_t bishopmoves =  bishopMoveTables[kingField][0];
        uint64_t rookmoves   =  rookMoveTables[kingField][0];
        const uint64_t kingSliderAttackFields = bishopmoves | rookmoves;

        bool mayBlockCheck = BIT64(mv->targetField) & kingSliderAttackFields;

        if (!mayBlockCheck && !(mv->captureType == knight) && !(mv->type == kingMove) && !(mv->type == enpassant)) {
            mv->sortEval = ILLEGAL;
            makeMove(mv, position);
            kingField = findLSB(position->pieceTables[1-position->toMove][king]);
            if (!isFieldAttacked(position, position->toMove, kingField)) {
                std::cout << chessPositionToFenString(*position) << std::endl;
                std::cout << chessPositionToOutputString(*position) << std::endl;
                undoMove(position);
                std::cout << chessPositionToFenString(*position) << std::endl;
                std::cout << "WTF??" << std::endl;
            }
            undoMove(position);
            return;
        }
    }*/

    isInCheck = !isInCheck;


    int16_t sortEval = 0;



    if (((uint16_t) mv->type) < 6) {
        sortEval = sortEval+captureEvals[mv->type][mv->captureType];
    }



    /*if (mv->captureType != none) {
        makeMove(mv, position);
        sortEval = SEE(position, mv);
        undoMove(position);
    }*/

    if (((uint16_t) mv->type) < 6) {
        //TODO: check this for endgames!!
        sortEval = sortEval+(getEarlyGamePSQentry(mv->type, position->toMove, mv->targetField)-getEarlyGamePSQentry(mv->type, position->toMove, mv->sourceField))/2;
    }

    if (mv->type == promotionQueen) {
        sortEval = sortEval+300;
    }

    bool sourceAttacked = false;
    bool targetAttacked = false;
    bool sourceCovered  = false;
    bool targetCovered  = false; //TODO: this doesnt work as intended - the target of a move is usually covered... - or may be covered after the move, eg. h7h5 from startposition, even if it wasnt before

    if (BIT64(mv->sourceField) & ownAttackTable->completeAttackTable) {
        sourceCovered = true;
    }
    if (BIT64(mv->targetField) & ownAttackTable->completeAttackTable) {
        targetCovered = true; //hm that can ONLY be false for some pawn moves actually...
    }

    if (BIT64(mv->sourceField) & opponentAttackTable->completeAttackTable) {
        sourceAttacked = true;
    }
    if (BIT64(mv->targetField) & opponentAttackTable->completeAttackTable) {
        targetAttacked = true;
    }

    if ((mv->captureType == none) && (mv->type > pawnMove) && (BIT64(mv->targetField) & opponentAttackTable->attackTables[pawn])) {
        sortEval = sortEval-100;
    }

    if (sourceCovered) {
        sortEval = sortEval-30;
    }

    if (targetCovered) {
        sortEval = sortEval+20;
    }

    if (sourceAttacked) {
        sortEval = sortEval+100;
        if (!sourceCovered) {
            sortEval = sortEval+100;
        }
        if (mv->type == kingMove) {
            sortEval = sortEval+400;
        }
    }

    if (targetAttacked  && (mv->captureType == none)) {
        sortEval = sortEval-100;
        if (!targetCovered) { //TODO: same problem as above, this never happens. But it still helps the ordering...
            sortEval = sortEval-100;
        }
    }

    if (targetAttacked && mv->type == kingMove) {
        sortEval = ILLEGAL;
        mv->sortEval = sortEval;
        return;
    }

    if ((mv->captureType != none)) {
        if ((BIT64(mv->targetField) & opponentAttackTable->completeAttackTable) == 0) {
            sortEval = sortEval+evalPars->figureValues[mv->captureType];
        } else {
            sortEval = sortEval+evalPars->figureValues[mv->captureType]/2;
        }
    }

    if (mv->type == rookMove) {
        uint16_t targetFile = FILE(mv->targetField);
        uint64_t pawns      = position->pieceTables[position->toMove][pawn];
        uint64_t opppawns      = position->pieceTables[1-position->toMove][pawn];
        if ((targetFile & pawns) == 0) {
            sortEval = sortEval+10;
            if ((targetFile & opppawns) == 0) {
                sortEval = sortEval+30;
            }
        }
    }

    if ((mv->type == pawnMove)) {
        if (BIT64(mv->targetField) & CENTER) {
            sortEval = sortEval+40;
        } else if (BIT64(mv->targetField) & WIDECENTER) {
            sortEval = sortEval+20;
        }
    }

    /*if (mv->type == pawnMove) {
        uint16_t target = mv->targetField;
        uint64_t ppMask = passedPawnMasks[position->toMove][target];
        uint64_t oppPawns = position->pieceTables[1-position->toMove][pawn];
        if ((ppMask & oppPawns) == 0) {
            sortEval = sortEval+50; //passed pawn push
        }
    }*/

    if ((position->madeMoves.length > 0) && (mv->captureType != none)) {
        chessMove previousMove = position->madeMoves[position->madeMoves.length-1];
        if (previousMove.targetField == mv->targetField) { //recapture is usually good
            sortEval = sortEval + 200;
        }
    }


    uint16_t moveHash = (mv->sourceField) | (mv->targetField << 8);

    if (moveHash == killerA) {
            sortEval = sortEval+120;
    }

    if (moveHash == killerB) {
            sortEval = sortEval+119;
    }

    if (mv->type == castlingKingside) {
        sortEval  = 110;
        if (kingBlockers[position->toMove] & opponentAttackTable->completeAttackTable) {
            sortEval = ILLEGAL;
            mv->sortEval = sortEval;
            return;
        }
    } else if (mv->type == castlingQueenside) {
        sortEval  = 70;
        if (queenBlockers[position->toMove] & opponentAttackTable->completeAttackTable) {
            sortEval = ILLEGAL;
            mv->sortEval = sortEval;
            return;
        }
    }

    if (mv->type == enpassant) {
        sortEval = 120;
    }

    if (mv->captureType == none) {
        int32_t hist = getHistoryTables()->getHistoryEntry(position->toMove, mv->sourceField, mv->targetField);

        int32_t historyValue = 2*std::sqrt(std::abs(hist)); //TODO: this is absolutely NOT a good idea performance-wise
        historyValue = (hist > 0 ? historyValue: -historyValue);


        if (historyValue > 128) {
            historyValue = 128;
        }

        if (historyValue < -128) {
            historyValue = -128;
        }

        sortEval = sortEval+historyValue;
        //std::cout << historyValue << std::endl;
        //std::cout << (historyTable[position->toMove][mv->sourceField][mv->targetField] >> 6) << std::endl;
    }

    if (mv->sourceField == refutationTarget) {
        sortEval = sortEval+100;
    }


    if (((mv->sourceField) | (mv->targetField << 8)) == hashedMove) {
            sortEval = sortEval+10000;
    }

    mv->sortEval = sortEval;
}

bool calculateStandardSortEvals(chessPosition* position,  vdt_vector<chessMove>* moves, uint16_t start_index, uint16_t ply, sortInfo sortinfo) {
    AttackTable opponentAttackTable = makeAttackTable(position, (playerColor) (1-position->toMove), position->pieceTables[position->toMove][king]);
    AttackTable ownAttackTable      = makeAttackTable(position, position->toMove);
    bool isInCheck      = ((opponentAttackTable.completeAttackTable & position->pieceTables[position->toMove][king]) != 0);
    int16_t bestEval = INT16_MIN;
    //uint16_t bestIndex = 0;
    killerTable* table = getKillerTable();
    const singlePlyKillers killers = table->getKillers(ply);
    const evalParameters* evalPars                      = getEvalParameters(); //TODO: move outside
    for (uint16_t ind = start_index; ind < moves->length; ind++) {
        calcSortEval(position, &(*moves)[ind], isInCheck, &opponentAttackTable, &ownAttackTable, sortinfo.hashMove, killers.killers[0], killers.killers[1], sortinfo.refutationTarget, evalPars);
        if ((*moves)[ind].sortEval > bestEval) {
            bestEval = (*moves)[ind].sortEval;
            //bestIndex = ind;
        }
    }

    /*chessMove buffer = (*moves)[0];
    (*moves)[0] = (*moves)[bestIndex];
    (*moves)[bestIndex] = buffer;*/
    //std::sort(moves->data, moves->data+moves->length);
    return isInCheck;
}

void orderCaptureMoves(chessPosition* position, vdt_vector<chessMove>* moves, uint16_t hashedMove) {
    if (moves->length == 0) {
        return;
    }

    int16_t bestEval = INT16_MIN;
    uint16_t bestIndex = 0;

    for (uint16_t ind = 0; ind < moves->length; ind++) {
        calcCaptureSortEval(position, &(*moves)[ind], hashedMove);
        if ((*moves)[ind].sortEval > bestEval) {
            bestEval = (*moves)[ind].sortEval;
            bestIndex = ind;
        }
    }

    chessMove buffer = (*moves)[0];
    (*moves)[0] = (*moves)[bestIndex];
    (*moves)[bestIndex] = buffer;

    //std::sort(moves->data, moves->data+moves->length);
}
