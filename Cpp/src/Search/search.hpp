/*
 * search.hpp
 *
 *  Created on: Sep 20, 2016
 *      Author: vabi
 */

#ifndef SEARCH_SEARCH_HPP_
#define SEARCH_SEARCH_HPP_
#include <assert.h>
#define HISTORY_CUTOFF (1ULL << 12)
#define NO_REFUTATION 64
void rescaleHistoryTable();
int16_t negamax(chessPosition* position,  uint16_t ply, uint16_t max_ply, int16_t depth, int16_t alpha, int16_t beta,  pvLine* PV, bool allowNullMove = true,  bool allowHashProbe = true);
int16_t negamaxQuiescence(chessPosition* position, uint16_t ply, int16_t alpha, int16_t beta, uint16_t depth);
int16_t root_search(chessPosition* position, chessMove* bestMove, int16_t alpha, int16_t beta, int16_t depth, uint16_t max_ply, vdt_vector<chessMove>* moves, uint64_t* nodeCounts);
void resetNodes();
bool calculateStandardSortEvals(chessPosition* position, vdt_vector<chessMove>* moves, uint16_t ply, uint16_t hashedMove, uint16_t refutationTarget);
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


class moveStack {
	chessMove moveArray[6000];
	uint16_t counter = 0;

public:
	vdt_vector<chessMove> getNext(){
		assert(counter < 35);
		counter++;
		//std::cout << counter << std::endl;
		return vdt_vector<chessMove>(moveArray+(counter-1)*150,150);
	}

	void release(){
		assert(counter > 0);
		counter--;
	}

	uint16_t getCounter(){
		return counter;
	}

	void reset(){
		counter = 0;
	}



};

searchDebugData getSearchData();
void resetSearchData();
bool isSearchValid();
void setTotalTime(uint32_t tTime, uint64_t start);

#endif /* SEARCH_SEARCH_HPP_ */
