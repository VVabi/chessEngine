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
#include <parameters/parameters.hpp>
int32_t evaluation(const chessPosition* position, int32_t alpha, int32_t beta);
int32_t pawnEvaluation(const chessPosition* position, uint8_t* pawnColumnOccupancy, uint16_t phase, const evalParameters* evalPars);
int32_t staticPawnEval(uint64_t pawns, playerColor color, const staticPawnEvalParameters* evalPars);
int32_t kingSafety(const chessPosition* position, const uint8_t* pawnColumnOccupancy, const AttackTable* whiteAttackTable, const AttackTable* blackAttackTable);
int32_t debugPawnEvaluation(const chessPosition* position, uint8_t* pawnColumnOccupancy, uint16_t phase, VDTpawnEvaluation* pawnEval);
int32_t debugEvaluation(const chessPosition* position, VDTevaluation* debugStruct);

struct evaluationResult {
	int16_t PSQ;
	int16_t staticPawn;
	int16_t passedPawn;
	int16_t kingSafety;
	int16_t bishoppair;
	int16_t mobility;
	int16_t rookOpenFiles;
};
evaluationResult getEvaluationResult();
#endif /* EVALUATION_EVALUATION_HPP_ */
