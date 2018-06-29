/*
 * evaluation.hpp
 *
 *  Created on: Sep 25, 2016
 *      Author: vabi
 */

#ifndef EVALUATION_EVALUATION_HPP_
#define EVALUATION_EVALUATION_HPP_

#include <lib/Attacks/attacks.hpp>
#include <parameters/parameters.hpp>


int32_t evaluation(const chessPosition* position, int32_t alpha, int32_t beta, bool PSQ_only = false);

int32_t staticPawnEval(uint64_t pawns, playerColor color, const staticPawnEvalParameters* evalPars);



int16_t passedPawnEval(int32_t* untaperedEval, uint64_t whitePawns, uint64_t blackPawns, uint16_t blackKing, uint16_t whiteKing, uint64_t whitePieces, uint64_t blackPieces);

void  getBackwardsPawns(uint64_t* whiteBackwards, uint64_t* blackBackwards, uint64_t wPawns, uint64_t bPawns);
uint8_t getPawnColumns(uint64_t in);
uint64_t getDoubledPawns(uint64_t in);
uint64_t getIsolatedPawns(uint64_t in);
int16_t staticPawnEvalComplete(const chessPosition* position);

int16_t kingSafety(const chessPosition* position, const evalParameters* par, const AttackTable* attackTables);
int16_t outposts(const chessPosition* position, const evalParameters* par, const AttackTable* attackTables);
int16_t trappedPieces(const chessPosition* position, const evalParameters* par, const AttackTable* attackTables);
int16_t rookOpenFiles(const chessPosition* position, const evalParameters* par, const AttackTable* attackTables);
int16_t staticPawnEvaluation(const chessPosition* position, const evalParameters* par, const AttackTable* attackTable);
int16_t bishopPair(const chessPosition* position, const evalParameters* par, const AttackTable* attackTables);

#endif /* EVALUATION_EVALUATION_HPP_ */
