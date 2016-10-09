/*
 * evaluation.hpp
 *
 *  Created on: Sep 25, 2016
 *      Author: vabi
 */

#ifndef EVALUATION_EVALUATION_HPP_
#define EVALUATION_EVALUATION_HPP_

#include <lib/Attacks/attacks.hpp>
#include <communication/gen/VDT.h>

int32_t evaluation(const chessPosition* position, int32_t alpha, int32_t beta);
int32_t pawnEvaluation(const chessPosition* position, uint8_t* pawnColumnOccupancy);
int32_t staticPawnEval(uint64_t pawns, playerColor color);
int32_t kingSafety(chessPosition* position, const uint8_t* pawnColumnOccupancy, const AttackTable* whiteAttackTable, const AttackTable* blackAttackTable);
int32_t debugPawnEvaluation(const chessPosition* position, uint8_t* pawnColumnOccupancy, VDTpawnEvaluation* pawnEval);
int32_t debugEvaluation(const chessPosition* position, VDTevaluation* debugStruct);

#endif /* EVALUATION_EVALUATION_HPP_ */
