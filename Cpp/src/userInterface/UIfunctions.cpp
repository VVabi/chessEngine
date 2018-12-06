/*
 * UIfunctions.cpp
 *
 *  Created on: Oct 9, 2016
 *      Author: vabi
 */

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <lib/basics.hpp>
#include <fstream>
#include <lib/bitfiddling.h>
#include <hashTables/hashTables.hpp>
#include <lib/DebugFunctions/debugFunctions.hpp>
#include "UIlayer.hpp"
#include <Search/search.hpp>
#include <lib/Attacks/attacks.hpp>
#include <lib/moveMaking/moveMaking.hpp>
#include <lib/moveGeneration/moveGeneration.hpp>
#include <algorithm>
#include <lib/Evaluation/evaluation.hpp>
#include <atomic>
#include <string>
#include <Search/repetition.hpp>
#include <userInterface/userInterface.hpp>
#include <vector>


static uint8_t searchId = 0;

uint64_t get_timestamp() {
    struct timeval start;
    uint64_t mtime, seconds, useconds;
    gettimeofday(&start, NULL);
    seconds  = start.tv_sec;
    useconds = start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    return mtime;
}

void search(chessPosition cposition, searchParameters params, bool quietMode = false); //TODO remove

uint64_t runSinglePositionPerformanceTest(std::string position, uint16_t depth, uint64_t* negamaxNodes, uint64_t* qNodes) {
    searchId++;
    std::vector<std::string> moves;
    memoryLibrarianAdd(position, moves);
    resetSearchData();
    resetQuiescenceNodes();
    struct timeval start, end;
    //long mtime, seconds, useconds;
    gettimeofday(&start, NULL);
    //int32_t eval = negamax(&c, depth, -100000, 100000, &bestMove);
    uint64_t start_ts = get_timestamp();
    setTotalTime(100000000, start_ts);
    chessPosition cposition = memoryLibrarianRetrievePosition();
    searchParameters paramsToUse;
    paramsToUse.depth = depth;
    paramsToUse.type = fixed_depth;

    search(cposition, paramsToUse, true);

    gettimeofday(&end, NULL);

    /*seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;*/
    searchDebugData data = getSearchData();
    uint64_t nodeCount = getQuiescenceNodes()+data.totalNodes;
    *negamaxNodes = data.totalNodes; //getNodes();
    *qNodes       = getQuiescenceNodes();
    //double nps = ((double) nodeCount)/((double) mtime)*1000.0;
    /*std::cout << "Searched " <<  nodeCount << " positions in " << mtime << " for " << nps << " nodes per second" << std::endl;
    std::cout << "Forced move " << moveToString(bestMove, c) << std::endl;
    std::cout << "Evaluation "  << eval << std::endl;*/

    return nodeCount;
}






std::string moveToString(chessMove move) {
    if (move.type == castlingKingside) {
            if (move.sourceField < 32) {
                return "e1g1";
            } else {
                return "e8g8";
            }
    }

    if (move.type == castlingQueenside) {
            if (move.sourceField < 32) {
                return "e1c1";
            } else {
                return "e8c8";
            }
    }

    uint16_t sourceField = move.sourceField;
    uint16_t targetField = move.targetField;

    char ret[4];
    ret[0] = FILE(sourceField)+'a';
    ret[1] = ROW(sourceField)+'1';
    ret[2] = FILE(targetField)+'a';
    ret[3] = ROW(targetField)+'1';

    std::string retString = "";
    retString.push_back(ret[0]);
    retString.push_back(ret[1]);
    retString.push_back(ret[2]);
    retString.push_back(ret[3]);

    if (move.type == promotionBishop) {
        retString.push_back('b');
    }
    if (move.type == promotionKnight) {
        retString.push_back('n');
    }
    if (move.type == promotionRook) {
        retString.push_back('r');
    }
    if (move.type == promotionQueen) {
        retString.push_back('q');
    }
    return retString;
}

std::string moveToExtendedString(chessMove move) {
    std::string ret = moveToString(move);
    ret = ret+" "+std::to_string((static_cast<int32_t>(move.type)))+" "+std::to_string((static_cast<int32_t>(move.captureType)));
    return ret;
}


uint64_t stringToMove(std::string mv) {
    char sourceColumn = mv[0]-'a';
    char sourceRow = mv[1]-'1';
    uint16_t sourceField = sourceRow*8+sourceColumn;
    char targetColumn = mv[2]-'a';
    char targetRow = mv[3]-'1';
    uint16_t targetField = targetRow*8+targetColumn;

    return BIT64(sourceField) | BIT64(targetField);
}




bool checkMove(chessPosition* position, std::string move, chessMove* out) {
    vdt_vector<chessMove> moves = vdt_vector<chessMove>(100);
            uint64_t mv = stringToMove(move);
            generateAllMoves(&moves, position);
            calculateStandardSortEvals(position, &moves, 0, 0, sortInfo(false, NO_REFUTATION, 0));
            std::sort(moves.data, moves.data+moves.length);
            bool found = false;
            chessMove m;
            for (uint16_t ind = 0; ind < moves.length; ind++) {
                if (moves[ind].sortEval < -10000) {
                    break;
                }

                uint64_t current = BIT64(moves[ind].sourceField) | BIT64(moves[ind].targetField);
                if (current == mv) {
                    if ((moves[ind].type > 8)) { //its a promotion
                        char type = move.at(4);
                        if ((type == 'r') && moves[ind].type != promotionRook) {
                            continue;
                        }
                        if ((type == 'q') && moves[ind].type != promotionQueen) {
                            continue;
                        }
                        if ((type == 'b') && moves[ind].type != promotionBishop) {
                            continue;
                        }
                        if ((type == 'n') && moves[ind].type != promotionKnight) {
                            continue;
                        }
                    }
                    found = true;
                    m = moves[ind];
                    break;
                }
                if (moves[ind].type == castlingKingside) {
                    if (position->toMove == white) {
                        if (move == "e1g1") {
                            found = true;
                            m = moves[ind];
                            break;
                        }
                    }
                    if (position->toMove == black) {
                        if (move == "e8g8") {
                            found = true;
                            m = moves[ind];
                            break;
                        }
                    }
                }
                if (moves[ind].type == castlingQueenside) {
                    if (position->toMove == white) {
                        if (move == "e1c1") {
                            found = true;
                            m = moves[ind];
                            break;
                        }
                    }
                    if (position->toMove == black) {
                        if (move == "e8c8") {
                            found = true;
                            m = moves[ind];
                            break;
                        }
                    }
                }
            }
            moves.free_array();
            *out = m;
            return found;
}


bool checkAndMakeMove(chessPosition* position, std::string move) {
    chessMove m;
    bool found = checkMove(position, move, &m);
        if (found) {
            //std::cout << moveToString(m) << std::endl;
            makeMove(&m, position);
            uint16_t kingField = findLSB(position->pieceTables[1-position->toMove][king]);
            if (isFieldAttacked(position, position->toMove, kingField)) {
                //std::cout << "Illegal move!" << std::endl;
                undoMove(position);
                return false;
            }
        } else {
            std::cout << "Invalid move!" << std::endl;
        }
        return found;
}




void outputUint64(uint64_t num) {
    for (int16_t row = 7; row > -1; row--) {
        for (uint16_t file = 0; file < 8; file++) {
            uint16_t field = 8*row+file;
            if (BIT64(field) & num) {
                std::cout << "1";
            } else {
                std::cout << "0";
            }
        }
        std::cout << std::endl;
    }
}


