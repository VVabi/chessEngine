/*
 * evaluation.hpp
 *
 *  Created on: Sep 25, 2016
 *      Author: vabi
 */

#ifndef EVALUATION_EVALUATION_HPP_
#define EVALUATION_EVALUATION_HPP_

int32_t evaluation(chessPosition* position);
int32_t pawnEvaluation(chessPosition* position, uint8_t* pawnColumnOccupancy);
int32_t staticPawnEval(uint64_t pawns, playerColor color);

#endif /* EVALUATION_EVALUATION_HPP_ */
