/*
 * toplevelSearch.cpp
 *
 *  Created on: Jun 24, 2017
 *      Author: vabi
 */


#include <list>
#include <string>
#include <lib/basics.hpp>
#include <userInterface/userInterface.hpp>
#include <userInterface/uciInterface.hpp>
#include <fstream>
#include <lib/moveMaking/moveMaking.hpp>
#include <lib/Evaluation/evaluation.hpp>
#include <Search/search.hpp>
#include <userInterface/interfaceStructs.hpp>
#include <lib/moveGeneration/moveGeneration.hpp>
#include <logging/logger.hpp>
#include <iostream>
#include <hashTables/hashTables.hpp>
#include <lib/bitfiddling.h>
#include <algorithm>
#include <userInterface/UIlayer.hpp>
#include <Search/killerMoves.hpp>
#include <tests/tests.hpp>


static uint8_t searchId;

uint32_t calcSearchTime(searchParameters params,  playerColor toMove, uint16_t numMadeMoves, uint32_t* worst_case_time) {
    if (params.type == infinite) {
        *worst_case_time = UINT32_MAX;
        return UINT32_MAX;
    }

    if (params.type == fixed_depth) {
        *worst_case_time = UINT32_MAX;
        return UINT32_MAX;
    }

    //TODO: implement the movesToGo parameter handling - currently we play stuff like "40 moves in 40 min" extremely badly
    if (params.totalTime[toMove] > 0) {
        uint32_t total = params.totalTime[toMove];
        uint32_t increment = params.increment[toMove];

        //estimate remaining moves
        uint16_t totalExpectedMoves = 1/2*numMadeMoves+60;

        int16_t remainingMoves = totalExpectedMoves-numMadeMoves;

        if (remainingMoves < 20) {
            remainingMoves = 20;
        }

        remainingMoves = std::min((int32_t) remainingMoves, (int32_t) params.movesToGo);

        uint32_t completeExpectedTime = total+(remainingMoves-1)*increment;
        float timeAllotted = 2*completeExpectedTime/(3.0*remainingMoves);

        double maxFraction = std::min(10.0, static_cast<double>(remainingMoves/2));

        if (timeAllotted > total/maxFraction) {
            timeAllotted = total/maxFraction;
        }
        *worst_case_time = total/1.5;
        return timeAllotted;
    }

    return 100; //some default
}


bool checkContinue(searchParameters params, uint16_t depth, uint16_t maxdepth, uint16_t passedTime, uint16_t allottedTime) {
    if (depth > maxdepth) {
        return false;
    }

    if (params.type == infinite) {
        return true;
    }

    if (params.type == fixed_depth) {
        return (params.depth >= depth) || (depth <= 3);
    }

    if (params.type == time_until_move) {
        return passedTime < allottedTime;
    }

    if (params.type == fixed_time) {
        return passedTime < params.fixedTime;
    }

    return true;
}


#define EXTENSIONS_ALLOWED 7  //TODO: investigate why going to 5 is not significantly better??


uint32_t cnt = 0;

uint32_t searchMove(chessPosition* position, chessMove* bestMove, uint32_t* nodeCount, uint64_t* mtime, int32_t* eval, bool doAspiration, searchParameters params, bool quietMode) {
    resetSearchData();
    resetQuiescenceNodes();
    uint64_t start_ts  = get_timestamp();
    uint16_t depth = 3;
    *eval = 0;
    int32_t alpha = -32000;
    int32_t beta  = 32000;
    searchId++;
    uint64_t searchedNodes = 0;

    uint32_t worst_case_search_time = 0;
    uint32_t totalTime = calcSearchTime(params, position->toMove, position->madeMoves.length, &worst_case_search_time);
    setTotalTime(worst_case_search_time, start_ts);
    uint16_t madeMoves = position->madeMoves.length;

    pvLine line;
    line.numMoves = 0;
    uint16_t  maxdepth = 27;

    //if there is only a single reply, don't continue searching - it is just a waste of time
    //-----------------------------------------------------------------------
    uint32_t numLegalMoves = perftNodes(position, 1);

    if (numLegalMoves == 1) {
        maxdepth = 3;
    }

    if (params.flexibleDepth) {
        uint16_t phase = position->totalFigureEval/100;

        if (position->pieceTables[white][queen] | position->pieceTables[black][queen]) {
            phase = phase+5;
        }

        if (phase < 40) {
            params.depth++;
        }

        if (phase < 20) {
            params.depth++;
        }

        if (phase < 10) {
            params.depth++;
        }
    }

    while (checkContinue(params, depth, maxdepth, get_timestamp()-start_ts, totalTime)) {
        try {
            *eval = negamax(position, plyInfo(0, depth+EXTENSIONS_ALLOWED, 0, depth), AlphaBeta(alpha, beta), &line, searchSettings(nullmove_enabled, hashprobe_disabled, checkextension_enabled, searchId));

            if (doAspiration) {
                if ((*eval <= alpha) || (*eval >= beta)) {
                    *eval = negamax(position, plyInfo(0, depth+EXTENSIONS_ALLOWED, 0, depth), AlphaBeta(-32000, 32000), &line, searchSettings(nullmove_enabled, hashprobe_disabled, checkextension_enabled, searchId));
                }

                alpha = *eval-50;
                beta  = *eval+50;
            }

            *bestMove = line.line[0];
            //TODO: free MEMORY!!!
        } catch(timeoutException& e) {
            break;
        }

        *mtime = get_timestamp()-start_ts;
        uint64_t totalNodes = 0;
        searchDebugData data = getSearchData();
        for (uint16_t ind = 0; ind < 40; ind++) {
            totalNodes = totalNodes+data.nodes[ind];
        }

        *nodeCount = (data.totalNodes+getQuiescenceNodes());

        std::list<std::string> moveList;

        for (uint16_t ind = 0; ind < line.numMoves; ind++) {
            std::string mv = moveToString(line.line[ind]);
            moveList.push_back(mv);
        }
        if (!quietMode) {
            sendSearchInfo(*nodeCount, *mtime, *eval, depth, moveList);
        }
        line.numMoves = 0;
        depth++;
        searchedNodes = searchedNodes+*nodeCount;
        if (*eval > 29000 || *eval < -29000) {
            int32_t mate_plys;
            if (*eval < 0) {
                mate_plys = *eval+30000;
            } else {
                mate_plys = 30000-*eval;
            }

            if (depth >= mate_plys) {
                break;
            }
        }
    }

    depth--;
    *mtime = get_timestamp()-start_ts;
    while (position->madeMoves.length > madeMoves) {
        chessMove current = position->madeMoves[position->madeMoves.length-1];

        if (current.sourceField == 0 && current.targetField == 0) {
            undoNullMove(position);
        } else {
            undoMove(position);
        }
    }
    return depth;
}
