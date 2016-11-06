/*
 * search.hpp
 *
 *  Created on: Sep 20, 2016
 *      Author: vabi
 */

#ifndef SEARCH_SEARCH_HPP_
#define SEARCH_SEARCH_HPP_

#define HISTORY_CUTOFF (1UL << 12)

void rescaleHistoryTable();
int16_t negamax(chessPosition* position, int16_t depth, int16_t alpha, int16_t beta, chessMove* bestMove, bool allowNullMove = true,  bool allowHashProbe = true);
int16_t negamaxQuiescence(chessPosition* position, int16_t alpha, int16_t beta, uint16_t depth);
void resetNodes();
bool orderStandardMoves(chessPosition* position, vdt_vector<chessMove>* moves, uint16_t depth, uint16_t hashedMove);
void orderCaptureMoves(chessPosition* position, vdt_vector<chessMove>* moves);
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

struct searchDebugData{
	uint64_t called;
	uint64_t wentToQuiescence;
	uint64_t fake_3fold_repetitions;
	uint64_t threefold_repetitions;
	uint64_t nodes[25];
	uint64_t totalNodes;
	uint64_t bestIndex[25][150];
	uint64_t futility_tried;
	uint64_t futility_successful;
	uint64_t wentToSearch;
	uint64_t neededSort;
	uint64_t nullMovePruningTried;
	uint64_t nullMovePruningSuccessful;

};

searchDebugData getSearchData();
void resetSearchData();

#endif /* SEARCH_SEARCH_HPP_ */
