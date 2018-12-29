/*
 * evaluation.cpp
 *
 *  Created on: Sep 25, 2016
 *      Author: vabi
 */

#include <stdint.h>
#include <lib/basics.hpp>
#include <lib/basicTypes.hpp>
#include <lib/Defines/boardParts.hpp>
#include <lib/Defines/pieceCombinations.hpp>
#include <lib/Evaluation/endgames/endgameEvals.hpp>
#include <lib/Evaluation/evaluation.hpp>
#include <lib/Evaluation/tapering.hpp>
#include <lib/figureValueHashing.hpp>
#include <map>

static EvaluationComponent evaluationComponents[] = {
        //Careful here - order is important since a previous function may write needed info to EvalMemory!
                            {       &evalMobility,          eval_mobility,                 256,        256,        256      },
                            {       &kingSafety,            eval_kingsafety,               256,        256,        256      },
                            {       &trappedPieces,         eval_trapped_pieces,           256,        256,        256      },
                            {       &outposts,              eval_outposts,                 256,        256,        256      },
                            {       &rookOpenFiles,         eval_rookfiles,                256,        256,        256      },
                            {       &doubledPawnEval,       eval_doubled_pawn,             256,        256,        256      },
                            {       &isolatedPawnEval,      eval_isolated_pawn,            256,        256,        256      },
                            {       &backwardPawnsEval,     eval_backwards_pawn,           256,        256,        256      },
                            {       &bishopPair,            eval_bishoppair,               256,        256,        256      },
                            {       &passedPawnEval,        eval_passed_pawns,             256,        256,        256      },
                            {       &kingEndgamePosition,   eval_king_endgame_position,    256,        256,        256      },
                            {       &spaceAdvantage,        eval_space,                    256,        256,        256      },
};

static SimpleEvaluationComponent simpleEvaluationComponents[] = {
                            {       &PSQ,             eval_PSQ,                            256,        256,        256      },
};

std::map<evaluationType, DetailedEvaluationResultComponent> getDetailedEvalResults(const chessPosition* position) {
    std::map<evaluationType, DetailedEvaluationResultComponent> ret;

    if (position->totalFigureEval < 700) {
        int16_t eval = 0;
        bool in_special_endgame = false;
        if (position->presentPieces.compare(WHITEKRK) || position->presentPieces.compare(BLACKKRK)) {
            eval = COLORSIGN(position->toMove)*KRK_endgame(position);
            in_special_endgame = true;
        }

        if (position->presentPieces.compare(WHITEKBBK) || position->presentPieces.compare(BLACKKBBK)) {
            eval = COLORSIGN(position->toMove)*KBBK_endgame(position);
            in_special_endgame = true;
        }

        if (position->presentPieces.compare(WHITEKBPK) || position->presentPieces.compare(BLACKKBPK)) {
            eval = COLORSIGN(position->toMove)*KBPK_endgame(position);
            in_special_endgame = true;
        }

        if (position->presentPieces.compare(WHITEKPK) || position->presentPieces.compare(BLACKKPK)) {
            eval = COLORSIGN(position->toMove)*KPK_endgame(position);
            in_special_endgame = true;
        }

        if (position->presentPieces.compare(WHITEKBNK) || position->presentPieces.compare(BLACKKBNK)) {
            eval = COLORSIGN(position->toMove)*KBNK_endgame(position);
            in_special_endgame = true;
        }

        if (in_special_endgame) {
            DetailedEvaluationResultComponent detailedResults;
            detailedResults.components.common       = 0;
            detailedResults.components.early_game   = 0;
            detailedResults.components.endgame      = eval;
            detailedResults.eval                    = eval*256;
            detailedResults.taperingValue           = 0;

            ret[eval_special_endgames] = detailedResults;
            DetailedEvaluationResultComponent dummy;
            dummy.components.common       = 0;
            dummy.components.early_game   = 0;
            dummy.components.endgame      = 0;
            dummy.eval                    = 0;
            dummy.taperingValue           = 0;
            ret[eval_draw_detection]      = dummy;

            for (uint16_t cnt=0; cnt < sizeof(simpleEvaluationComponents)/sizeof(SimpleEvaluationComponent); cnt++) {
                const SimpleEvaluationComponent component    = simpleEvaluationComponents[cnt];
                ret[component.type]                          = dummy;
            }

            for (uint16_t cnt=0; cnt < sizeof(evaluationComponents)/sizeof(EvaluationComponent); cnt++) {
                const EvaluationComponent component             = evaluationComponents[cnt];
                ret[component.type]                             = dummy;
            }

            return ret;
        }
    }

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
    EvalMemory evalMemory;
    //now the subtler components
    //---------------------------
    for (uint16_t cnt=0; cnt < sizeof(evaluationComponents)/sizeof(EvaluationComponent); cnt++) {
        const EvaluationComponent component    = evaluationComponents[cnt];
        EvalComponentResult evalValue          = component.evalFunction(position, evalPars, &evalMemory);

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

    EvalMemory evalMemory;

    //now the subtler components
    //---------------------------
    for (uint16_t cnt=0; cnt < sizeof(evaluationComponents)/sizeof(EvaluationComponent); cnt++) {
        const EvaluationComponent component    = evaluationComponents[cnt];
        EvalComponentResult evalValue          = component.evalFunction(position, evalPars, &evalMemory);

        eval = eval+((tapering*((int32_t) evalValue.early_game))/256)*component.coefficient_earlygame;
        eval = eval+(((256-tapering)*((int32_t) evalValue.endgame))/256)*component.coefficient_endgame;
        eval = eval+((int32_t) evalValue.common)*component.coefficient_common;
        //TODO: add check whether all eval values that should be zero are zero
    }

    eval = eval/256;

    if (position->toMove == white) {
        eval = eval+10;
    } else {
        eval = eval-10;
    }

#ifdef RANDOMEVAL
    eval = eval+(rand() & 7)-3; //TODO: how is this performance-wise? // NOLINT
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

    if (position->presentPieces.maskedCompare(KKBB, pawn)) {
        uint64_t whiteBishops = position->pieceTables[white][bishop];
        uint64_t blackBishops = position->pieceTables[black][bishop];

        bool wbishopOnWhite = !!(whiteBishops & WHITEFIELDS);
        bool bbishopOnWhite = !!(blackBishops & WHITEFIELDS);

        bool differentColored = wbishopOnWhite != bbishopOnWhite;

        if (differentColored) {
            eval = eval/2;
        }
    }

    return COLORSIGN(position->toMove)*eval;
}
