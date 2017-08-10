/*
 * rootSearch.cpp
 *
 *  Created on: Nov 6, 2016
 *      Author: vabi
 */
#ifdef NDEF

#include <iostream>
#include <lib/basics.hpp>
#include <lib/moveMaking/moveMaking.hpp>
#include <tests/tests.hpp>
#include <DataTypes/vdt_vector.hpp>
#include <lib/moveGeneration/moveGeneration.hpp>
#include <lib/Attacks/attacks.hpp>
#include <lib/bitfiddling.h>
#include "search.hpp"
#include <hashTables/hashTables.hpp>
#include <algorithm>
#include <assert.h>
#include <lib/Evaluation/evaluation.hpp>
#include <fstream>
#include <userInterface/UIlayer.hpp>
#include <atomic>
extern int32_t historyTable[2][64][64];
extern uint8_t searchId;
extern std::atomic<bool> continueSearch;
static searchDebugData searchCounts;
extern searchDebugData searchCounts;
extern uint64_t bishopFieldTable[];
extern uint64_t rookFieldTable[];
extern uint16_t killerMoves[40][2];

static inline void get_extensions_reductions(uint16_t* reduction, uint16_t* extension, bool check, bool movingSideInCheck, uint16_t ply, uint16_t max_ply, int16_t depth, chessMove* move, uint16_t ind) {

        if(!check && !movingSideInCheck && (move->captureType == none) && (depth > 2) && (ply > 0)) {
            if((ind > 3)) {
                *reduction = 1;
                if(move->sortEval < -50) {
                    *reduction = 2;
                }
            }
        }



        if(check && ((ply+depth < max_ply-1) || ((depth == 1) && (ply+depth < max_ply)) )) {
            *extension = 1;
            *reduction = 0;
        }

        bool closeToPromotion = (move->type == pawnMove) && ( (move->targetField > 48) || (move->targetField < 16));

        if( (closeToPromotion || (move->type == promotionQueen)) && (ply+depth+*extension < max_ply-1)) {
            *extension = *extension+1;
            *reduction = 0;
        }

}

static inline void handleBetaCutoff(chessMove* bestMove, uint64_t zobristHash, int16_t beta, int16_t depth, uint16_t ply, uint8_t searchId) {
    setHashEntry(FAILHIGH, beta, depth, searchId, (bestMove->sourceField | (bestMove->targetField << 8)), zobristHash);
    if(bestMove->captureType == none) {
        uint16_t toRemember = (bestMove->sourceField | (bestMove->targetField << 8));
        if ( (killerMoves[ply][0] != toRemember)) {
            killerMoves[ply][1] = killerMoves[ply][0];
            killerMoves[ply][0] = toRemember;
        }
    }
}

uint64_t getCurrentNodeCount() {
    uint64_t totalNodes=0;
    searchDebugData data = getSearchData();
    for(uint16_t ind=0; ind < 25; ind++) {
        totalNodes = totalNodes+data.nodes[ind];
    }

    return (data.totalNodes+getQuiescenceNodes());
}

int16_t root_search(chessPosition* position, chessMove* bestMove, int16_t alpha, int16_t beta, int16_t depth, uint16_t max_ply, vdt_vector<chessMove>* moves, uint64_t* nodeCounts) {

    uint16_t ply = 0;
    assert(alpha < beta);
    assert(ply+depth <= max_ply);
    uint64_t ownKing = position->pieceTables[position->toMove][king];
    bool movingSideInCheck = isFieldAttacked(position, (playerColor) (1-position->toMove), findLSB(ownKing));
    bool legalMovesAvailable = false;
    bool foundGoodMove = false;
    searchCounts.wentToSearch++;

    //remembers the index of best move after sorting
    //-------------------------------------------
    int16_t bestIndex = -1;



    uint64_t currentCount = getCurrentNodeCount();

    for(uint16_t ind=0; ind < moves->length; ind++) {
        //illegal move. Since list is sorted or, in case ind=0, best move is first, we can leave here: all further moves are also illegal.
        //---------------------------------------------------------------------------------------------------------------------------------
        if((*moves)[ind].sortEval < -10000) {
            break;
        }

        //now make move and check for legality
        //---------------------------------------
        makeMove(&(*moves)[ind], position);
        uint16_t kingField = findLSB( position->pieceTables[1- position->toMove][king]);

        if(movingSideInCheck || (BIT64((*moves)[ind].sourceField) & (rookFieldTable[kingField] | bishopFieldTable[kingField]))) {
            if(isFieldAttacked( position,  position->toMove, kingField)) {
                //move exposed our king, undo and continue
                //---------------------------------------------
                assert((*moves)[ind].type != kingMove); //all king moves moving into check should be found by move ordering!
                undoMove(position);
                continue;
            }
        }

#ifdef DEBUG
        //debug check: if we overlooked an illegal move, we scream here
        //-------------------------------------------------------------
        if(isFieldAttacked( position,  position->toMove, kingField)) {
            std::cout << chessPositionToOutputString(*position);
            std::cout <<  (rookFieldTable[kingField] | bishopFieldTable[kingField]);
            std::cout << "WTF" << std::endl;
        }
#endif
        searchCounts.nodes[depth]++;
        searchCounts.totalNodes++;
        legalMovesAvailable = true;
        chessMove mv;
        uint16_t ownkingField = findLSB( position->pieceTables[position->toMove][king]);

        //check whether last move gave check (so its a check-check)
        //----------------------------------------------------------------
        bool check = isFieldAttacked(position,  (playerColor) (1-position->toMove), ownkingField);

        //get extensions and reductions
        //------------------------------
        uint16_t reduction = 0;
        uint16_t extension = 0;
        get_extensions_reductions(&reduction, &extension, check, movingSideInCheck, ply, max_ply, depth, &(*moves)[ind], ind);

        //PVSearch, currently a small gain for us with the > 3
        //-------------------------------------------------
        if(((ind > 3) || foundGoodMove )&& (depth > 2)) {
            int32_t value = -negamax(position, ply+1, max_ply, depth-1-reduction+extension, -alpha-1, -alpha, &mv);
            if(value < alpha+1) {
                undoMove(position);
                uint64_t newCurrentNodes = getCurrentNodeCount();
                nodeCounts[ind]  = newCurrentNodes-currentCount;
                currentCount = newCurrentNodes;
                /*std::cout << "Move " << moveToString((*moves)[ind], *position) << " sort eval " << (*moves)[ind].sortEval << std::endl;
                std::cout << "Nodecount " << nodeCounts[ind] << std::endl;*/
                continue;
            }
        }

        //this is the real, full-fledged search now
        //-------------------------------------------
        int32_t value = -negamax(position, ply+1, max_ply, depth-1+extension, -beta, -alpha, &mv);

        //in case move is better than previous, remember
        //------------------------------------------------
        if(value > alpha) {
            foundGoodMove = true;
            alpha = value;
            *bestMove = (*moves)[ind];
            bestIndex = ind;
        }

        undoMove(position);

        uint64_t newCurrentNodes = getCurrentNodeCount();
        nodeCounts[ind]  = newCurrentNodes-currentCount;
        currentCount = newCurrentNodes;
        /*std::cout << "Move " << moveToString((*moves)[ind], *position) << " sort eval " << (*moves)[ind].sortEval << std::endl;
        std::cout << "Nodecount " << nodeCounts[ind] << std::endl;*/
        //in case of beta cutoff, leave
        //----------------------------------
        if((alpha >= beta)) {
            handleBetaCutoff(bestMove, position->zobristHash, beta, depth, ply, searchId);
            if(bestIndex != -1) {
                searchCounts.bestIndex[depth][bestIndex]++;
            }
            uint64_t newCurrentNodes = getCurrentNodeCount();
            nodeCounts[ind]  = newCurrentNodes-currentCount;
            currentCount = newCurrentNodes;
            return beta;
        }
    }

    //no cutoff. Check whether we found a good move, mate/stalemate handling accordingly. Then Cleanup/bookkeeping, then return alpha
    //-------------------------------------------------
    /*if(bestIndex != -1) {
        updateHistoryTables(bestMove, depth, &moves, bestIndex, position->toMove);
        searchCounts.bestIndex[depth][bestIndex]++;
    }*/

    //mate scores originate here!
    //------------------------------
    if(!legalMovesAvailable) {
        if(movingSideInCheck) {
            alpha = -30000+ply;
        } else {
            alpha = 0;
        }
    }

    //set hash entry
    //----------------------
    if(foundGoodMove) {
        setHashEntry(FULLSEARCH, alpha, depth, searchId, (bestMove->sourceField | (bestMove->targetField << 8)), position->zobristHash);
    } else { //we failed low, remember as well
        setHashEntry(FAILLOW, alpha, depth, searchId, 0, position->zobristHash);
    }
    //and finally return alpha
    //--------------------------
    return alpha;
}

#endif
