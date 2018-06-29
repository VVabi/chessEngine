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

enum evaluationType {eval_kingsafety, eval_trapped_pieces, eval_outposts, eval_rookfiles, eval_static_pawns, eval_bishoppair};
enum taperingDirection {taper_none, taper_endgame_higher, taper_earlygame_higher};

struct EvaluationComponent {
    int16_t (*evalFunction)(const chessPosition* position, const evalParameters* par, const AttackTable* attackTables);
    evaluationType type;
    taperingDirection taperDirection;
};

EvaluationComponent evaluationComponents[] = {
                            {       &kingSafety,            eval_kingsafety,              taper_earlygame_higher},
                            {       &trappedPieces,         eval_trapped_pieces,          taper_none},
                            {       &outposts,              eval_outposts,                taper_none},
                            {       &rookOpenFiles,         eval_rookfiles,               taper_none},
                            {       &staticPawnEvaluation,  eval_static_pawns,            taper_none},
                            {       &bishopPair,            eval_bishoppair,              taper_none},
};


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

    //evaluation from the point of view of WHITE, sign changed in the end if necessary
    //--------------------------------------------------------------------------------------
    int32_t eval = 0;
    int32_t bufferMidgame =  position->pieceTableEval & 0xFFFF;
    bufferMidgame         =  bufferMidgame - (1 << 15);
    int32_t bufferEndgame =  position->pieceTableEval >> 16;
    bufferEndgame  = bufferEndgame-(1 << 14);
    uint16_t phase = position->totalFigureEval/100;

    if (position->pieceTables[white][queen] | position->pieceTables[black][queen]) {
        phase = phase+5;
    }

    uint16_t tapering = getTaperingValue(phase);
    int32_t pieceTableEval = ((256-tapering)*bufferEndgame+tapering*bufferMidgame)/256; //division by 256
    eval = eval+pieceTableEval;

    if (PSQ_only) {
        return (1-2*position->toMove)*eval;
    }

    int32_t evalsigned = (1-2*position->toMove)*eval;

    if ((evalsigned < alpha - 500) || (evalsigned > beta+500)) {
        //std::cout << "Futility pruning in eval" << std::endl;
        return evalsigned;
    }

    int16_t mobilityScore = 0;

    AttackTable attackTables[2];

    attackTables[white] = makeAttackTableWithMobility(position, white, &mobilityScore);
    eval = eval+mobilityScore;
    mobilityScore = 0;
    attackTables[black] = makeAttackTableWithMobility(position, black, &mobilityScore);
    eval = eval-mobilityScore;

    for (uint16_t cnt=0; cnt < sizeof(evaluationComponents)/sizeof(EvaluationComponent); cnt++) {
        const EvaluationComponent component    = evaluationComponents[cnt];
        int32_t evalValue                = component.evalFunction(position, evalPars, attackTables);
        if (component.taperDirection == taper_earlygame_higher) {
            evalValue = (tapering*evalValue)/256;
        } else if (component.taperDirection == taper_endgame_higher) {
            evalValue = ((256-tapering)*evalValue)/256;
        }
        eval = eval+evalValue;
    }



    int32_t untapered = 0;
    int32_t passedPawns = passedPawnEval(&untapered, position->pieceTables[white][pawn], position->pieceTables[black][pawn], findLSB(position->pieceTables[black][king]), findLSB(position->pieceTables[white][king]), position->pieces[white], position->pieces[black]);

    int16_t passedPawnPhase = std::max((int32_t) phase, 0);
    passedPawns = ((256-getTaperingValue(passedPawnPhase))*passedPawns)/256;
    eval = eval+passedPawns+untapered;

    if (position->toMove == white) {
        eval = eval+10;
    } else {
        eval = eval-10;
    }

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
