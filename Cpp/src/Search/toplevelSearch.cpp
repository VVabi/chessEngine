/*
 * toplevelSearch.cpp
 *
 *  Created on: Jun 24, 2017
 *      Author: vabi
 */




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
extern uint8_t searchId;
extern uint16_t killerMoves[40][2];

uint32_t calcSearchTime(searchParameters params,  playerColor toMove, uint16_t numMadeMoves, uint32_t* worst_case_time) {

    if(params.type == infinite) {
        *worst_case_time = UINT32_MAX;
        return UINT32_MAX;
    }

    if(params.type == fixed_depth) {
        *worst_case_time = UINT32_MAX;
        return UINT32_MAX;
    }

    //TODO: implement the movesToGo parameter handling - currently we play stuff like "40 moves in 40 min" extremely badly
    if(params.totalTime[toMove] > 0) {
        uint32_t total = params.totalTime[toMove];
        uint32_t increment = params.increment[toMove];

        //estimate remaining moves
        uint16_t totalExpectedMoves = 1/2*numMadeMoves+60;

        int16_t remainingMoves = totalExpectedMoves-numMadeMoves;

        if(remainingMoves < 20) {
            remainingMoves = 20;
        }

        uint32_t completeExpectedTime = total+remainingMoves*increment;
        float timeAllotted = 2*completeExpectedTime/(3.0*remainingMoves);

        if(timeAllotted > total/10.0) {
            timeAllotted = total/10.0;
        }
        *worst_case_time = total/10.0;
        return timeAllotted;
    }

    return 100; //some default
}


bool checkContinue(searchParameters params, uint16_t depth, uint16_t passedTime, uint16_t allottedTime) {

    if(depth > 27) {
        return false;
    }

    if(params.type == infinite) {
        return true;
    }

    if(params.type == fixed_depth) {
        return (params.depth >= depth) || (depth <= 3);
    }

    if(params.type == time_until_move) {
        return passedTime < allottedTime;
    }

    if(params.type == fixed_time) {
        return passedTime < params.fixedTime;
    }


    return true;
}


#define EXTENSIONS_ALLOWED 7  //TODO: investigate why going to 5 is not significantly better??

uint32_t searchMove(chessPosition* position, chessMove* bestMove, uint32_t* nodeCount, uint64_t* mtime, int32_t* eval, bool doAspiration, searchParameters params) {
/*#ifdef EXPERIMENTAL

    fenLogger.open("fen.txt", std::ios::app);

#endif*/

    //TODO: refactor this function
    memset(killerMoves, 0, 40*2*sizeof(uint16_t));
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
    vdt_vector<chessMove> moves = vdt_vector<chessMove>(150);
    generateAllMoves(&moves, position);
    uint16_t refutationTarget = NO_REFUTATION;
    calculateStandardSortEvals(position, &moves, 0, 0, sortInfo(false, refutationTarget, getHashMove(position->zobristHash)));
    std::stable_sort(moves.data, moves.data+moves.length);
    /*for(uint16_t ind=0; ind < moves.length; ind++) {
        std::cout<< moveToString(moves[ind], *position) << " Eval " << moves[ind].sortEval << std::endl;
    }*/

    pvLine line;
    line.numMoves = 0;
    while(checkContinue(params, depth, get_timestamp()-start_ts, totalTime)) {
        try {
            //std::cout << "Depth " << depth << std::endl;

            *eval = negamax(position, plyInfo(0, depth+EXTENSIONS_ALLOWED, 0, depth), AlphaBeta(alpha, beta), &line, searchSettings(nullmove_enabled, hashprobe_disabled, checkextension_enabled, searchId));
            if(doAspiration) {
                if ((*eval <= alpha) || (*eval >= beta)) {

                    *eval = negamax(position, plyInfo(0, depth+EXTENSIONS_ALLOWED, 0, depth), AlphaBeta(-32000, 32000), &line, searchSettings(nullmove_enabled, hashprobe_disabled, checkextension_enabled, searchId));
                }

                alpha = *eval-50;
                beta  = *eval+50;
            }

            *bestMove = line.line[0];
            /*chessMove localBestMove;

            uint64_t* nodeCounts = new uint64_t[moves.length];
            for(uint16_t ind=0; ind < moves.length; ind++) {
                nodeCounts[ind] = 0;
            }


            *eval =  root_search(position, &localBestMove, alpha, beta, depth, depth+3, &moves, nodeCounts);
            if(doAspiration) {
                if ((*eval <= alpha) || (*eval >= beta)) {
                    for(uint16_t ind=0; ind < moves.length; ind++) {
                        nodeCounts[ind] = 0;
                    }
                    *eval = root_search(position, &localBestMove, -32000, 32000, depth, depth+3, &moves, nodeCounts);
                }

                alpha = *eval-50;
                beta  = *eval+50;
            }

            *bestMove = localBestMove;

            uint64_t maxCount = 0;

            for(uint16_t ind=0; ind < moves.length; ind++) {

                if(nodeCounts[ind] > maxCount) {
                    maxCount = nodeCounts[ind];
                }

            }
            uint32_t scaling = 1;

            if(maxCount > INT16_MAX) {
                scaling = maxCount/10000+1;
            }
            for(uint16_t ind=0; ind < moves.length; ind++) {

                if(nodeCounts[ind] > 0) {
                    moves[ind].sortEval = (nodeCounts[ind]/scaling);
                } else {
                    if(moves[ind].sortEval > 0) {
                        moves[ind].sortEval = moves[ind].sortEval-500;
                    }
                }

                if(moves[ind].move == bestMove->move) {
                    moves[ind].sortEval = INT16_MAX;
                }
                //std::cout << moves[ind].sortEval << std::endl;
            }

            std::stable_sort(moves.data, moves.data+moves.length);*/

            /*std::cout << "New move list" << std::endl;

            for(uint16_t ind=0; ind < moves.length; ind++) {
                std::cout<< moveToString(moves[ind], *position) << " Eval " << moves[ind].sortEval << std::endl;
            }*/

            //TODO: free MEMORY!!!


        } catch(timeoutException& e) {
            //std::cout << "Search timed out" << std::endl;
            break;
        }

        *mtime = get_timestamp()-start_ts;
        uint64_t totalNodes=0;
        searchDebugData data = getSearchData();
        for(uint16_t ind=0; ind < 40; ind++) {
            totalNodes = totalNodes+data.nodes[ind];
        }

        *nodeCount = (data.totalNodes+getQuiescenceNodes());

        std::list<std::string> moveList;

        for(uint16_t ind=0; ind < line.numMoves; ind++) {
            std::string mv = moveToString(line.line[ind]);
            moveList.push_back(mv);
        }

        sendSearchInfo(*nodeCount, *mtime, *eval, depth, moveList);
        //logSearch(*nodeCount, *mtime, *eval, depth, moveToString(*bestMove));
        line.numMoves = 0;
        depth++;
        searchedNodes = searchedNodes+*nodeCount;
        if(*eval > 29000) {
            break;
        }
    }

    depth--;
    *mtime = get_timestamp()-start_ts;
    while(position->madeMoves.length > madeMoves) {
        chessMove current = position->madeMoves[position->madeMoves.length-1];

        if(current.sourceField == 0 && current.targetField == 0) {
            undoNullMove(position);
        } else {
            undoMove(position);
        }
    }
    return depth;
}
