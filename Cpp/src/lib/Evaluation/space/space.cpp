/*
 * space.cpp
 *
 *  Created on: Dec 27, 2018
 *      Author: vabi
 */


#include "lib/basics.hpp"
#include "lib/bitfiddling.h"
#include "parameters/parameters.hpp"
#include "lib/Attacks/attacks.hpp"
#include "lib/Evaluation/evaluation.hpp"
#include <algorithm>


int16_t evals[] = {0, 5, 10, 18, 30, 45, 60, 75, 90};

EvalComponentResult spaceAdvantage(const chessPosition* position, const evalParameters* evalParams __attribute__((unused)), EvalMemory* evalMemory  __attribute__((unused))) {

    EvalComponentResult ret;

    int32_t numWhitePieces = popcount(position->pieces[white] & NORTHHALF);
    int32_t numBlackPieces = popcount(position->pieces[black] & SOUTHHALF);

    numWhitePieces = std::min(8, numWhitePieces);
    numBlackPieces = std::min(8, numBlackPieces);

    ret.early_game = 0;
    ret.common = (evals[numWhitePieces]-evals[numBlackPieces])/3;
//#ifdef EXPERIMENTAL
    ret.endgame = (evals[numWhitePieces]-evals[numBlackPieces])/3;
//#endif
    return ret;
}
