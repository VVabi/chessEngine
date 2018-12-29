/*
 * space.cpp
 *
 *  Created on: Dec 27, 2018
 *      Author: vabi
 */


#include <DataTypes/data_utils.hpp>
#include "lib/basics.hpp"
#include "lib/bitfiddling.h"
#include "parameters/parameters.hpp"
#include "lib/Attacks/attacks.hpp"
#include "lib/Evaluation/evaluation.hpp"
#include <algorithm>

EvalComponentResult spaceAdvantage(const chessPosition* position, const evalParameters* evalParams, EvalMemory* evalMemory  __attribute__((unused))) {
    EvalComponentResult ret;

    uint32_t numWhitePieces = popcount(position->pieces[white] & NORTHHALF);
    uint32_t numBlackPieces = popcount(position->pieces[black] & SOUTHHALF);

    constexpr uint32_t maxSize = sizeof(evalParams->spaceParameters.figuresInOppHalf)/sizeof(evalParams->spaceParameters.figuresInOppHalf[0]);

    numWhitePieces = std::min(maxSize, numWhitePieces);
    numBlackPieces = std::min(maxSize, numBlackPieces);

    ret.early_game      = 0;
    int16_t whiteScore  = safe_access<const int16_t, maxSize>(evalParams->spaceParameters.figuresInOppHalf, numWhitePieces);
    int16_t blackScore  = safe_access<const int16_t, maxSize>(evalParams->spaceParameters.figuresInOppHalf, numBlackPieces);
    int16_t score       = (whiteScore-blackScore)/3;
    ret.common          = score;
    ret.endgame         = score;

    return ret;
}
