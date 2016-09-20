/*
 * search.hpp
 *
 *  Created on: Sep 20, 2016
 *      Author: vabi
 */

#ifndef SEARCH_SEARCH_HPP_
#define SEARCH_SEARCH_HPP_
int32_t negamax(chessPosition* position, uint16_t depth, int32_t alpha, int32_t beta, chessMove* bestMove);

void resetNodes();

uint32_t getNodes();



#endif /* SEARCH_SEARCH_HPP_ */
