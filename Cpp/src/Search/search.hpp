/*
 * search.hpp
 *
 *  Created on: Sep 20, 2016
 *      Author: vabi
 */

#ifndef SEARCH_SEARCH_HPP_
#define SEARCH_SEARCH_HPP_
#include <assert.h>
#include <userInterface/interfaceStructs.hpp>
#define HISTORY_CUTOFF (1ULL << 12)
#define NO_REFUTATION 64


enum NullmoveSetting            {nullmove_enabled, nullmove_disabled};
enum HashprobeSetting           {hashprobe_enabled, hashprobe_disabled};
enum CheckextensionSetting      {checkextension_enabled, checkextension_disabled};

struct plyInfo {
    uint16_t ply;
    uint16_t max_ply;
    uint16_t qply;
    int16_t depth;

    plyInfo increment(int16_t depth_adaption) {
        return plyInfo(ply+1, max_ply, qply, depth-1+depth_adaption);
    }

    plyInfo(uint16_t p, uint16_t m, uint16_t q, uint16_t d): ply(p), max_ply(m), qply(q), depth(d) {};

};

struct sortInfo {
    bool movingSideInCheck;
    uint16_t refutationTarget;
    uint16_t hashMove;
    sortInfo(bool m, uint16_t r, uint16_t h): movingSideInCheck(m), refutationTarget(r), hashMove(h) {};
};

struct searchSettings {
    NullmoveSetting nullmoveSetting;
    HashprobeSetting hashprobeSetting;
    CheckextensionSetting checkextensionSetting;
    uint8_t searchId;
    searchSettings(NullmoveSetting nms, HashprobeSetting hps, CheckextensionSetting ces, uint8_t sid): nullmoveSetting(nms), hashprobeSetting(hps), checkextensionSetting(ces), searchId(sid) {};
    explicit searchSettings(uint8_t sid):  nullmoveSetting(nullmove_enabled), hashprobeSetting(hashprobe_enabled), checkextensionSetting(checkextension_enabled), searchId(sid) {};
};

struct AlphaBeta {
    int16_t alpha;
    int16_t beta;
    AlphaBeta(): alpha(INT16_MIN), beta(INT16_MAX) {};
    AlphaBeta(int16_t a, int16_t b): alpha(a), beta(b) {};
    bool update(int16_t value) {
        bool ret = value > alpha? true: false;
        if(ret) {
            alpha=value;
        }
        return ret;
    }

    AlphaBeta invert() {
        return AlphaBeta(-beta, -alpha);
    }

    AlphaBeta zeroWindow() {
        return AlphaBeta(alpha, alpha+1);
    }

    bool betacutoff() {
        return alpha >= beta;
    }

    void sanityCheck() {
        assert(alpha <= beta);
    }

    void stalemate() {
        alpha = 0;
    }

    void mate(uint16_t ply) {
        alpha = -30000+ply;
    }

};

struct searchLoopResults {
    AlphaBeta alphabeta;
    int16_t bestIndex;
    uint16_t numLegalMoves;
    searchLoopResults(AlphaBeta ab, int16_t b, uint16_t n): alphabeta(ab), bestIndex(b), numLegalMoves(n) {};

    bool noMovesAvailable() {
        return numLegalMoves == 0;
    }

    bool foundGoodMove() {
        return (bestIndex >= 0);
    }
};

int16_t negamax(chessPosition* position, plyInfo ply, AlphaBeta alphabeta,  pvLine* PV, searchSettings settings);
int16_t negamaxQuiescence(chessPosition* position, uint16_t qply, uint16_t ply, AlphaBeta alphabeta, uint16_t depth, uint8_t searchId);
int16_t root_search(chessPosition* position, chessMove* bestMove, int16_t alpha, int16_t beta, int16_t depth, uint16_t max_ply, vdt_vector<chessMove>* moves, uint64_t* nodeCounts);
void resetNodes();
bool calculateStandardSortEvals(chessPosition* position, vdt_vector<chessMove>* moves, uint16_t start_index, uint16_t ply, sortInfo sortinfo);
void orderCaptureMoves(chessPosition* position, vdt_vector<chessMove>* moves, uint16_t hashedMove);
uint32_t getNodes();

uint32_t getQuiescenceNodes();
void resetQuiescenceNodes();

void resetIndices();

uint32_t getCalled();

void resetCalled();
uint32_t getSortCalled();
void resetSortCalled();
int32_t* getIndexCounts();
uint32_t getqCalled();

void resetqIndices();
int32_t* getqIndexCounts();
void resetqCalled();
uint32_t getSortqCalled();
void resetSortqCalled();

struct searchDebugData {
    uint64_t called;
    uint64_t wentToQuiescence;
    uint64_t fake_3fold_repetitions;
    uint64_t threefold_repetitions;
    uint64_t nodes[40];
    uint64_t totalNodes;
    uint64_t bestIndex[40][150];
    uint64_t futility_tried;
    uint64_t futility_successful;
    uint64_t wentToSearch;
    uint64_t neededSort;
    uint64_t nullMovePruningTried;
    uint64_t nullMovePruningSuccessful;
};


class moveStack {
    chessMove moveArray[7500];
    uint16_t counter = 0;

public:
    vdt_vector<chessMove> getNext() {
        assert(counter < 45);
        counter++;
        //std::cout << counter << std::endl;
        return vdt_vector<chessMove>(moveArray+(counter-1)*150, 150);
    }

    void release() {
        assert(counter > 0);
        counter--;
    }

    uint16_t getCounter() {
        return counter;
    }

    void reset() {
        counter = 0;
    }
};

searchDebugData getSearchData();
void resetSearchData();
bool isSearchValid();
void setTotalTime(uint32_t tTime, uint64_t start);
uint32_t calcSearchTime(searchParameters params,  playerColor toMove, uint16_t numMadeMoves, uint32_t* worst_case_time);
#endif /* SEARCH_SEARCH_HPP_ */
