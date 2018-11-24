/*
 * UIlayer.cpp
 *
 *  Created on: Oct 9, 2016
 *      Author: vabi
 */

#include <Search/history.hpp>
#include <lib/basics.hpp>
#include <userInterface/userInterface.hpp>
#include <userInterface/uciInterface.hpp>
#include <fstream>
#include <lib/moveMaking/moveMaking.hpp>
#include <lib/Evaluation/evaluation.hpp>
#include "UIlayer.hpp"
#include <Search/search.hpp>
#include <iomanip>
#include <userInterface/interfaceStructs.hpp>
#include <lib/moveGeneration/moveGeneration.hpp>
#include <logging/logger.hpp>
#include <iostream>
#include "userEvents.hpp"
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <atomic>
#include <thread>
#include <tests/tests.hpp>
#include <mutex>
#include <algorithm>
#include <hashTables/hashTables.hpp>
#include <lib/bitfiddling.h>
#include <parameters/parameters.hpp>
#include <parameters/parametersPrivate.hpp>
#include <parameters/externalParamReader.hpp>
#include <string>
#include <Search/killerMoves.hpp>
#include "userInterface/json/json.h"

//necessary to get windows compile to run
//----------------------------------------
#include <userInterface/mingw.thread.h>
#include <userInterface/mingw.mutex.h>



template <typename T>
T StringToNumber(const std::string &Text) {
    std::stringstream ss(Text);
    T result;
    return ss >> result ? result : 0;
}

std::mutex m;

void putLine(std::string output) {
    m.lock();
    std::cout << output << std::endl;
    m.unlock();
}

void logError(std::string msg) {
    putLine("ERROR: "+msg);
}

searchParameters paramsToUse;
std::atomic<bool> continueSearch;

bool doContinueSearch() {
    return continueSearch;
}

void sendSearchInfo(uint64_t nodes, uint32_t time, int32_t eval, uint32_t depth, const std::list<std::string>& PV) {
    double nps = (static_cast<double>(nodes))/(static_cast<double>(time))*1000.0;
    uint64_t npsInt = nps;
    std::stringstream out;


    if (eval > 29000) {
        int16_t mate_in = (30000-eval+1)/2;
        out << "info depth " << depth << " score mate " << mate_in << " nps " << npsInt << " nodes " << nodes << " pv ";
    } else if (eval < -29000) {
        int16_t mate_in = (-30000-eval)/2;
        out << "info depth " << depth << " score mate " << mate_in << " nps " << npsInt << " nodes " << nodes << " pv ";
    } else {
        out << "info depth " << depth << " score cp " << eval << " nps " << npsInt << " nodes " << nodes << " pv ";
    }

    for (auto iterator = PV.begin(); iterator != PV.end(); iterator++) {
        out << *iterator << " ";
    }
    putLine(out.str());
}



/*#ifdef EXPERIMENTAL
std::ofstream fenLogger;
#endif*/





void setSearchParams(searchParameters params) {
    paramsToUse = params;
}

std::thread engineThread;
std::atomic<bool> isSearching(false);

void search(chessPosition cposition, searchParameters params) {
    chessMove bestMove;
    uint32_t nodeCount = 0;
    uint64_t mtime = 0;
    int32_t eval = 0;
    searchMove(&cposition, &bestMove, &nodeCount, &mtime, &eval, false, params);
    putLine("bestmove " + moveToString(bestMove));
    free_position(&cposition);
    isSearching = false;
}

void launchSearch() {
    continueSearch = true;
    if (engineThread.joinable()) {
        engineThread.join();
    }
    isSearching = true;
    chessPosition cposition = memoryLibrarianRetrievePosition();
    engineThread = std::thread(search, cposition, paramsToUse);
}

void handleUciInput(std::ostream& stream) {
    stream << "id name Vabi" << std::endl;
    stream << "id author Fabian Lenhardt" << std::endl;
    stream << "uciok" << std::endl;
}

void handleIsReady(std::ostream& stream) {
    stream << "readyok" <<std::endl;
}

void handleClear() {
    killerTable* ktable = getKillerTable();
    ktable->clear();
    clearHashTables();
    HistoryTables* table = getHistoryTables();
    table->clearHistoryTable();
}

void handleStop() {
    continueSearch = false;
    if (engineThread.joinable()) {
        engineThread.join();
    }
    assert(!isSearching);
}

void handleGo(std::list<std::string> input) {
    searchParameters params;
    params.type = unknown;
    params.depth = 0;
    params.fixedTime = 0;
    params.totalTime[0] = 0;
    params.totalTime[1] = 0;
    params.increment[0] = 0;
    params.increment[1] = 0;
    params.movesToGo = UINT16_MAX;

    for (auto iterator = input.begin(), end = input.end(); iterator != end; ++iterator) {
        std::string current = *iterator;
        if ("wtime" == current) {
            iterator++;
            if (iterator == input.end()) {
                putLine("Missing parameter for " + current);
                params.type = unknown;
                break;
            }
            std::string time = *iterator;
            params.totalTime[0] = StringToNumber<int32_t>(time);
            params.type = time_until_move;
        }
        if ("btime" == current) {
            iterator++;
            if (iterator == input.end()) {
                putLine("Missing parameter for " + current);
                params.type = unknown;
                break;
            }
            std::string time = *iterator;
            params.totalTime[1] = StringToNumber<int32_t>(time);
            params.type = time_until_move;
        }
        if ("winc" == current) {
            iterator++;
            if (iterator == input.end()) {
                putLine("Missing parameter for " + current);
                params.type = unknown;
                break;
            }
            std::string time = *iterator;
            params.increment[0] = StringToNumber<int32_t>(time);
            params.type = time_until_move;
        }
        if ("binc" == current) {
            iterator++;
            if (iterator == input.end()) {
                putLine("Missing parameter for " + current);
                params.type = unknown;
                break;
            }
            std::string time = *iterator;
            params.increment[1] = StringToNumber<int32_t>(time);
            params.type = time_until_move;
        }

        if ("infinite" == current) {
            params.type = infinite;
        }

        if ("movetime" == current) {
            iterator++;
            if (iterator == input.end()) {
                putLine("Missing parameter for " + current);
                params.type = unknown;
                break;
            }
            std::string time = *iterator;
            params.fixedTime = StringToNumber<int32_t>(time);
            params.type = fixed_time;
        }

        if ("depth" == current) {
            iterator++;
            if (iterator == input.end()) {
                putLine("Missing parameter for " + current);
                params.type = unknown;
                break;
            }
            std::string d =  *iterator;
            params.depth = StringToNumber<int32_t>(d);
            params.type = fixed_depth;
        }


        if ("movestogo" == current) {
            iterator++;
            if (iterator == input.end()) {
                putLine("Missing parameter for " + current);
                params.type = unknown;
                break;
            }
            std::string togo = *iterator;
            params.movesToGo = StringToNumber<int32_t>(togo);
            params.type = time_until_move;
        }
    }

    if (params.type != unknown) {
        setSearchParams(params);
        launchSearch();
    }
}

void handlePerft(std::list<std::string> input) {
        auto iterator = input.begin();
        if (iterator != input.end()) {
            uint16_t depth = StringToNumber<int32_t>(*iterator);
            chessPosition p = memoryLibrarianRetrievePosition();
            uint32_t perftret = perftNodes(&p, depth);
            free_position(&p);
            std::stringstream ret;
            ret << "perftresult " << perftret;
            putLine(ret.str());
        }
}


void handlePosition(std::list<std::string> input) {
    std::string fen = "";
    if (input.empty()) {
        //std::cout << "Invalid position request" << std::endl;
        return;
    }
    auto iterator = input.begin();
    if ("startpos" == *iterator) {
        fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        iterator++;
    } else {
        if (*iterator != "fen") {
            putLine("Invalid position request");
            return;
        }
        iterator++;
        fen = *iterator;
        iterator++;
        while ((iterator != input.end()) && ("moves" != *iterator)) {
            if ("fen" != *iterator) {
            fen = fen+" "+*iterator;
            }
            iterator++;
        }
    }
    std::vector<std::string> moveList;
    while (iterator != input.end()) {
        if ("moves" != *iterator) {
            moveList.push_back(*iterator);
        }
        iterator++;
    }

    memoryLibrarianAdd(fen, moveList);
}

// eval_kingsafety, eval_trapped_pieces, eval_outposts, eval_rookfiles, eval_static_pawns, eval_bishoppair, eval_PSQ, eval_passed_pawns, eval_mobility

std::string toString(const evaluationType type) {
    switch (type) {
        case eval_kingsafety:
            return "king safety";
            break;
        case eval_outposts:
            return "outposts";
            break;
        case eval_trapped_pieces:
            return "trapped pieces";
            break;
        case eval_rookfiles:
            return "rook files";
            break;
        case eval_doubled_pawn:
            return "doubled pawn";
            break;
        case eval_isolated_pawn:
            return "isolated pawn";
            break;
        case eval_backwards_pawn:
            return "backwards pawn";
            break;
        case eval_bishoppair:
            return "bishop pair";
            break;
        case eval_PSQ:
            return "psq";
            break;
        case eval_passed_pawns:
            return "passed pawns";
            break;
        case eval_mobility:
            return "mobility";
            break;
    }

    return ""; //Unreachable
}
void handlecheckquiet() {
    chessPosition cposition = memoryLibrarianRetrievePosition();
    int32_t eval = evaluation(&cposition, -32000, 32000);
    AlphaBeta alphabeta;
    alphabeta.alpha = -32000;
    alphabeta.beta  =  32000;
    int32_t qeval = negamaxQuiescence(&cposition, 0, 0, alphabeta, 0, 0);

    if (eval == qeval) {
        putLine("true");
    } else {
        putLine("false");
    }
    free_position(&cposition);
}


void handleEval() {
    chessPosition cposition = memoryLibrarianRetrievePosition();
    int32_t eval = evaluation(&cposition, -32000, 32000);

    if (cposition.toMove == black) {
        eval = -eval; //always from POV of white
    }

    auto evalMap = getDetailedEvalResults(&cposition);

    Json::Value root;

    root["total"] = eval;
    for (auto const& entry : evalMap) {
        if (entry.first == eval_mobility) {
            root[toString(entry.first)] = entry.second.eval;
        } else {
            root[toString(entry.first)] = entry.second.eval/256;
        }


    }
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = "";  // or whatever you like
    putLine(Json::writeString(builder, root));
    free_position(&cposition);
}

Json::Value evalComponentResultToJson(const EvalComponentResult comp) {
    Json::Value value;
    value["common"]             = comp.common;
    value["earlygame"]          = comp.early_game;
    value["endgame"]            = comp.endgame;
    return value;
}

Json::Value detailedEvalComponentToJson(const DetailedEvaluationResultComponent comp) {
    Json::Value value;

    value["taperingValue"]  = comp.taperingValue;
    value["eval"]           = comp.eval;
    value["components"]     = evalComponentResultToJson(comp.components);
    return value;
}

void handleDetailedEval() {
    chessPosition cposition = memoryLibrarianRetrievePosition();
    int32_t eval = evaluation(&cposition, -32000, 32000);

    if (cposition.toMove == black) {
        eval = -eval; //always from POV of white
    }

    auto evalMap = getDetailedEvalResults(&cposition);

    Json::Value root;

    root["total"] = eval;
    for (auto const& entry : evalMap) {
        root[toString(entry.first)] = detailedEvalComponentToJson(entry.second);
    }
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = "";
    putLine(Json::writeString(builder, root));
    free_position(&cposition);
}


void handlePawnEval() {
    std::cout << "NOT IMPLEMENTED" << std::endl;
    /*chessPosition cposition = memoryLibrarianRetrievePosition();
    std::cout << staticPawnEvalComplete(&cposition) << std::endl;
    free_position(&cposition);*/
}


void handleSEE(std::list<std::string> input) {
    chessPosition cposition = memoryLibrarianRetrievePosition();

    chessMove m;
    if (checkMove(&cposition, input.front(), &m)) {
        std::cout << SEE(&cposition, &m) << std::endl;
    } else {
        std::cout << "not a valid move" << std::endl;
    }
}

void handleSetEvalParam(std::list<std::string> input) {
    auto iterator = input.begin();
    std::string key = *iterator;
    iterator++;
    std::string valueStr = *iterator;
    int64_t value   = std::stol(valueStr);
    parameterReader reader = parameterReader();
    preParameters* pars = getPreParameters();
    reader.changeValue(pars, key, value);
    initializeDependentParameters(*pars);
}



void runPerformanceTests(uint32_t d) {
    for (uint16_t depth = 3; depth < d+1; depth++) {
        std::ifstream file;
        file.open("chesspositionsfixed.txt");
        std::string line;
        uint64_t negamaxNodes = 0;
        uint64_t qNodes = 0;

        uint32_t nodes = 0;
        while (std::getline(file, line)) {
            //std::cout << line << std::endl;
            uint64_t nmNodes = 0;
            uint64_t qn = 0;
            uint64_t newNodes = runSinglePositionPerformanceTest(line, depth, &nmNodes, &qn, true);
            if (newNodes != qn+nmNodes) {
                std::cout << "WTF???" << std::endl;
            }
            nodes = nodes + newNodes;
            //std::cout << newNodes << std::endl;
            //std::cout << nmNodes << std::endl;
            //std::cout << qn << std::endl;
            negamaxNodes = negamaxNodes+nmNodes;
            qNodes = qNodes+qn;
        }
        std::cout << "Depth " << depth  << " Nodes " << nodes << std::endl;
        std::cout << "negamaxnodes " <<  negamaxNodes << std::endl;
        std::cout << "qnodes " <<  qNodes << std::endl;
        file.close();
    }
    isSearching = false;
}

void handleRunPerformanceTests(std::list<std::string> input) {
    auto iterator = input.begin();
    std::string valueStr = *iterator;
    int32_t value   = std::stoi(valueStr);
    continueSearch = true;
    if (engineThread.joinable()) {
        engineThread.join();
    }
    isSearching = true;
    engineThread = std::thread(runPerformanceTests, value);
}

void handleInfo() {
#ifdef DEBUG
    putLine("DEBUG");
#else
    putLine("RELEASE");
#endif
}

void handleGetMoveOrdering() {
    vdt_vector<chessMove> vec = vdt_vector<chessMove>(200);
    chessPosition cposition = memoryLibrarianRetrievePosition();
    generateAllMoves(&vec, &cposition);
    calculateStandardSortEvals(&cposition, &vec, 0, 0, sortInfo(false, NO_REFUTATION, 0));
    std::stable_sort(vec.data, vec.data+vec.length);//stable sort makes the engine 100% predictable and comparable between different optimization levels

    for (uint16_t ind = 0; ind < vec.length; ind++) {
        std::cout << moveToExtendedString(vec[ind]) << " " << vec[ind].sortEval << std::endl;
    }

    vec.free_array();
}

void UIloop() {
    initUserEvents();
    bool continueLoop = true;
    handleClear();
    while (continueLoop) {
        userEvent ev = getNextUserEvent();

        //restricted interface during search
        //------------------------------------
        if (isSearching) {
            switch (ev.input) {
                case isready:
                    handleIsReady(std::cout);
                    break;
                case stop:
                    handleStop();
                    break;
                case quit:
                    handleStop();
                    continueLoop = false;
                    break;
                default:
                    putLine("Invalid request during search");
            }

        } else {
            //full interface otherwise
            //---------------------------
            switch (ev.input) {
                case uci:
                    handleUciInput(std::cout);
                    break;
                case isready:
                    handleIsReady(std::cout);
                    break;
                case go:
                    handleGo(ev.data);
                    break;
                case position:
                    handlePosition(ev.data);
                    break;
                case stop:
                    handleStop();
                    break;
                case quit:
                    handleStop();
                    continueLoop = false;
                    break;
                case perft:
                    handlePerft(ev.data);
                    break;
                case eval:
                    handleEval();
                    break;
                case detailedeval:
                    handleDetailedEval();
                    break;
                case pawnEval:
                    handlePawnEval();
                    break;
                case invalid:
                    putLine("Invalid request");
                    break;
                case see:
                    handleSEE(ev.data);
                    break;
                case clear:
                    handleClear();
                    break;
                case info:
                    handleInfo();
                    break;
                case setEvalParam:
                    handleSetEvalParam(ev.data);
                    break;
                case performanceTests:
                    handleRunPerformanceTests(ev.data);
                    break;
                case getMoveOrdering:
                    handleGetMoveOrdering();
                    break;
                case checkquiet:
                    handlecheckquiet();
                    break;
                default:
                    putLine("Not yet implemented");
            }
        }
    }
}



