/*
 * evaluation.hpp
 *
 *  Created on: Sep 25, 2016
 *      Author: vabi
 */

#ifndef EVALUATION_EVALUATION_HPP_
#define EVALUATION_EVALUATION_HPP_

#include <lib/Attacks/attacks.hpp>
int32_t evaluation(chessPosition* position, int32_t alpha, int32_t beta);
int32_t pawnEvaluation(chessPosition* position, uint8_t* pawnColumnOccupancy);
int32_t staticPawnEval(uint64_t pawns, playerColor color);
int32_t kingSafety(chessPosition* position, const uint8_t* pawnColumnOccupancy, const AttackTable* whiteAttackTable, const AttackTable* blackAttackTable);

#endif /* EVALUATION_EVALUATION_HPP_ */
