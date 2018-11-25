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
#include <map>


struct EvalComponentResult {
    int16_t common;
    int16_t early_game;
    int16_t endgame;

    EvalComponentResult() {
        common      = 0;
        early_game  = 0;
        endgame     = 0;
    }
};

enum evaluationType {eval_kingsafety, eval_trapped_pieces, eval_outposts, eval_rookfiles, eval_doubled_pawn, eval_isolated_pawn, eval_backwards_pawn, eval_bishoppair, eval_PSQ, eval_passed_pawns, eval_mobility};


struct EvaluationComponent {
    EvalComponentResult (*evalFunction)(const chessPosition* position, const evalParameters* par, const AttackTable* attackTables);
    evaluationType type;
    int16_t coefficient_common;
    int16_t coefficient_earlygame;
    int16_t coefficient_endgame;
};

struct SimpleEvaluationComponent {
    EvalComponentResult (*evalFunction)(const chessPosition* position, const evalParameters* par);
    evaluationType type;
    int16_t coefficient_common;
    int16_t coefficient_earlygame;
    int16_t coefficient_endgame;
};

struct DetailedEvaluationResultComponent {
    EvalComponentResult components;
    int32_t eval;
    int16_t taperingValue;
};


int32_t evaluation(const chessPosition* position, int32_t alpha, int32_t beta, bool PSQ_only = false);
std::map<evaluationType, DetailedEvaluationResultComponent> getDetailedEvalResults(const chessPosition* position);
int32_t staticPawnEval(uint64_t pawns, playerColor color, const staticPawnEvalParameters* evalPars);




void  getBackwardsPawns(uint64_t* whiteBackwards, uint64_t* blackBackwards, uint64_t wPawns, uint64_t bPawns);
uint8_t getPawnColumns(uint64_t in);
uint64_t getDoubledPawns(uint64_t in);
uint64_t getIsolatedPawns(uint64_t in);
EvalComponentResult doubledPawnEval(const chessPosition* position, const evalParameters* par, const AttackTable* attackTables);
EvalComponentResult isolatedPawnEval(const chessPosition* position, const evalParameters* par, const AttackTable* attackTables);
EvalComponentResult backwardPawnsEval(const chessPosition* position, const evalParameters* par, const AttackTable* attackTables);

EvalComponentResult kingSafety(const chessPosition* position, const evalParameters* par, const AttackTable* attackTables);
EvalComponentResult outposts(const chessPosition* position, const evalParameters* par, const AttackTable* attackTables);
EvalComponentResult trappedPieces(const chessPosition* position, const evalParameters* par, const AttackTable* attackTables);
EvalComponentResult rookOpenFiles(const chessPosition* position, const evalParameters* par, const AttackTable* attackTables);
EvalComponentResult staticPawnEvaluation(const chessPosition* position, const evalParameters* par, const AttackTable* attackTable);
EvalComponentResult bishopPair(const chessPosition* position, const evalParameters* par, const AttackTable* attackTables);
EvalComponentResult passedPawnEval(const chessPosition* position, const evalParameters* par, const AttackTable* attackTables);

EvalComponentResult PSQ(const chessPosition* position, const evalParameters* par);

AttackTable makeAttackTableWithMobility(const chessPosition* position, playerColor attackingSide, EvalComponentResult* result);

#endif /* EVALUATION_EVALUATION_HPP_ */
