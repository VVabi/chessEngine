/*
 * search.hpp
 *
 *  Created on: Sep 20, 2016
 *      Author: vabi
 */

#ifndef SEARCH_SEARCH_HPP_
#define SEARCH_SEARCH_HPP_
int32_t negamax(chessPosition* position, uint16_t depth, int32_t alpha, int32_t beta, chessMove* bestMove);
int32_t negamaxQuiescence(chessPosition* position, int32_t alpha, int32_t beta, uint16_t depth);
void resetNodes();
bool orderStandardMoves(chessPosition* position, vdt_vector<chessMove>* moves);
void orderCaptureMoves(chessPosition* position, vdt_vector<chessMove>* moves);
uint32_t getNodes();

uint32_t getQuiescenceNodes();
void resetQuiescenceNodes();

void resetIndices();

int32_t* getIndexCounts();

#endif /* SEARCH_SEARCH_HPP_ */
