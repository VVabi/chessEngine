/*

 * evaluation.cpp
 *
 *  Created on: Sep 25, 2016
 *      Author: vabi
 */

#include <stdlib.h>     /* srand, rand */

#include "lib/basics.hpp"
#include "evaluation.hpp"
#include "lib/Attacks/attacks.hpp"
#include "lib/bitfiddling.h"
#include "userInterface/UIlayer.hpp"
#include "iostream"
#include "fstream"
#include "parameters/parameters.hpp"
#include "logging/logger.hpp"
#include "lib/Defines/chessFields.hpp"
#include "lib/Evaluation/tapering.hpp"
#include "lib/Evaluation/endgames/endgameEvals.hpp"
#include "lib/Defines/pieceCombinations.hpp"
#include "lib/Evaluation/evaluation.hpp"
#include <map>

#define MOBILTIYWEIGHT 256
EvaluationComponent evaluationComponents[] = {
                            {       &kingSafety,            eval_kingsafety,            taper_earlygame_higher,                     256,        256,        256                                     },
                            {       &trappedPieces,         eval_trapped_pieces,        taper_none,                                 256,        256,        256                                     },
                            {       &outposts,              eval_outposts,              taper_none,                                 256,        256,        256                                     },
                            {       &rookOpenFiles,         eval_rookfiles,             taper_none,                                 256,        256,        256                                     },
                            {       &staticPawnEvaluation,  eval_static_pawns,          taper_none,                                 256,        256,        256                                     },
                            {       &bishopPair,            eval_bishoppair,            taper_none,                                 256,        256,        256                                     },
                            {       &passedPawnEval,        eval_passed_pawns,          taper_none  | taper_endgame_higher,         256,        256,        256                                     },
};

SimpleEvaluationComponent simpleEvaluationComponents[] = {
                            {       &PSQ,             eval_PSQ,           taper_earlygame_higher | taper_endgame_higher,            256,        256,        256              },
};


std::map<evaluationType, DetailedEvaluationResultComponent> getDetailedEvalResults(const chessPosition* position) {
    std::map<evaluationType, DetailedEvaluationResultComponent> ret;

    const evalParameters* evalPars = getEvalParameters();
    uint16_t phase = position->totalFigureEval/100;

    if (position->pieceTables[white][queen] | position->pieceTables[black][queen]) {
        phase = phase+5;
    }

    uint16_t tapering = getTaperingValue(phase);

    for (uint16_t cnt=0; cnt < sizeof(simpleEvaluationComponents)/sizeof(SimpleEvaluationComponent); cnt++) {
        const SimpleEvaluationComponent component    = simpleEvaluationComponents[cnt];
        EvalComponentResult evalValue                = component.evalFunction(position, evalPars);
        int32_t eval = 0;
        eval = eval+((tapering*((int32_t) evalValue.early_game))/256)*component.coefficient_earlygame;
        eval = eval+(((256-tapering)*((int32_t) evalValue.endgame))/256)*component.coefficient_endgame;
        eval = eval+((int32_t) evalValue.common)*component.coefficient_common;

        DetailedEvaluationResultComponent detailedResults;
        detailedResults.components      = evalValue;
        detailedResults.eval            = eval;
        detailedResults.taperingValue   = tapering;
        ret[component.type] = detailedResults;
    }

    //now the subtler components
    //---------------------------
    int16_t mobilityScore = 0;

    AttackTable attackTables[2];
    int16_t eval = 0;
    attackTables[white] = makeAttackTableWithMobility(position, white, &mobilityScore);
    int32_t rawmobility = mobilityScore;
    eval = eval+((int32_t) mobilityScore)*256;

    mobilityScore = 0;
    attackTables[black] = makeAttackTableWithMobility(position, black, &mobilityScore);

    eval = eval-((int32_t) mobilityScore)*256;
    rawmobility = rawmobility-mobilityScore;
    DetailedEvaluationResultComponent detailedResults;
    detailedResults.components.common  = rawmobility;
    detailedResults.eval               = rawmobility;
    detailedResults.taperingValue      = tapering;
    ret[eval_mobility] = detailedResults;

    for (uint16_t cnt=0; cnt < sizeof(evaluationComponents)/sizeof(EvaluationComponent); cnt++) {
        const EvaluationComponent component    = evaluationComponents[cnt];
        EvalComponentResult evalValue          = component.evalFunction(position, evalPars, attackTables);

        int32_t eval = 0;
        DetailedEvaluationResultComponent detailedResults;
        eval = eval+((tapering*((int32_t) evalValue.early_game))/256)*component.coefficient_earlygame;
        eval = eval+(((256-tapering)*((int32_t) evalValue.endgame))/256)*component.coefficient_endgame;
        eval = eval+((int32_t) evalValue.common)*component.coefficient_common;

        detailedResults.components      = evalValue;
        detailedResults.eval            = eval;
        detailedResults.taperingValue   = tapering;

        ret[component.type] = detailedResults;
    }

    return ret;
}


int32_t evaluation(const chessPosition* position, int32_t alpha, int32_t beta, bool PSQ_only) {
    if ((position->pieceTables[white][pawn] == 0) &&
        (position->pieceTables[black][pawn] == 0) &&
        position->totalFigureEval < 400) {
        return 0; //insufficent material
    }

    //specific endgames
    //----------------------
    if (position->totalFigureEval < 700) {
        if (position->presentPieces.compare(WHITEKRK) || position->presentPieces.compare(BLACKKRK)) {
            return COLORSIGN(position->toMove)*KRK_endgame(position);
        }

        if (position->presentPieces.compare(WHITEKBBK) || position->presentPieces.compare(BLACKKBBK)) {
            return COLORSIGN(position->toMove)*KBBK_endgame(position);
        }

        if (position->presentPieces.compare(WHITEKBPK) || position->presentPieces.compare(BLACKKBPK)) {
            return COLORSIGN(position->toMove)*KBPK_endgame(position);
        }

        if (position->presentPieces.compare(WHITEKPK) || position->presentPieces.compare(BLACKKPK)) {
            return COLORSIGN(position->toMove)*KPK_endgame(position);
        }

        if (position->presentPieces.compare(WHITEKBNK) || position->presentPieces.compare(BLACKKBNK)) {
            return COLORSIGN(position->toMove)*KBNK_endgame(position);
        }
    }

    //now normal eval
    //-----------------
    const evalParameters* evalPars = getEvalParameters();
    uint16_t phase = position->totalFigureEval/100;

    if (position->pieceTables[white][queen] | position->pieceTables[black][queen]) {
        phase = phase+5;
    }

    uint16_t tapering = getTaperingValue(phase);

    //evaluation from the point of view of WHITE, sign changed in the end if necessary
    //--------------------------------------------------------------------------------------

    int32_t eval = 0;

    //Simple eval components first
    //--------------------------------
    for (uint16_t cnt=0; cnt < sizeof(simpleEvaluationComponents)/sizeof(SimpleEvaluationComponent); cnt++) {
        const SimpleEvaluationComponent component    = simpleEvaluationComponents[cnt];
        EvalComponentResult evalValue                = component.evalFunction(position, evalPars);
        eval = eval+((tapering*((int32_t) evalValue.early_game))/256)*component.coefficient_earlygame;
        eval = eval+(((256-tapering)*((int32_t) evalValue.endgame))/256)*component.coefficient_endgame;
        eval = eval+((int32_t) evalValue.common)*component.coefficient_common;
        //TODO: add check whether all eval values that should be zero are zero
    }

    int32_t evalsigned = (1-2*position->toMove)*(eval/256);

    if (PSQ_only || (evalsigned < alpha - 500) || (evalsigned > beta+500)) {
        return evalsigned;
    }

    //now the subtler components
    //---------------------------
    int16_t mobilityScore = 0;

    AttackTable attackTables[2];

    attackTables[white] = makeAttackTableWithMobility(position, white, &mobilityScore);
    eval = eval+((int32_t) mobilityScore)*MOBILTIYWEIGHT;
    mobilityScore = 0;
    attackTables[black] = makeAttackTableWithMobility(position, black, &mobilityScore);
    eval = eval-((int32_t) mobilityScore)*MOBILTIYWEIGHT;

    for (uint16_t cnt=0; cnt < sizeof(evaluationComponents)/sizeof(EvaluationComponent); cnt++) {
        const EvaluationComponent component    = evaluationComponents[cnt];
        EvalComponentResult evalValue          = component.evalFunction(position, evalPars, attackTables);

        eval = eval+((tapering*((int32_t) evalValue.early_game))/256)*component.coefficient_earlygame;
        eval = eval+(((256-tapering)*((int32_t) evalValue.endgame))/256)*component.coefficient_endgame;
        eval = eval+((int32_t) evalValue.common)*component.coefficient_common;
        //TODO: add check whether all eval values that should be zero are zero
    }

    eval = eval/256;

    /*if (position->toMove == white) {
        eval = eval+10;
    } else {
        eval = eval-10;
    }*/

#ifdef RANDOMEVAL
    eval = eval+(rand() & 7)-3; //TODO: how is this performance-wise?
#endif


    //try to detect dead draws
    //-----------------------
    if (position->totalFigureEval < 500) {
        if ((position->pieceTables[white][pawn] == 0) && (eval > 0)) {
            if ((position->pieceTables[white][rook] | position->pieceTables[white][queen]) == 0) {
                eval = eval/16;
            }
        }

        if ((position->pieceTables[black][pawn] == 0) && (eval < 0)) {
            if ((position->pieceTables[black][rook] | position->pieceTables[black][queen]) == 0) {
                eval = eval/16;
            }
        }
    }

    return COLORSIGN(position->toMove)*eval;
}
