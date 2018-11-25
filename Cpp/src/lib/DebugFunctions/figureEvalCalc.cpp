/*
 * evalCalculation.cpp
 *
 *  Created on: Sep 19, 2016
 *      Author: vabi
 */


#include <lib/basics.hpp>
#include <lib/basicTypes.hpp>
#include <lib/bitfiddling.h>
#include <parameters/parameters.hpp>

int16_t calcFigureEvaluation(const chessPosition* position) {
    int16_t val = 0;
    for (uint16_t ind = 0; ind < 5; ind++) {
        uint64_t whitePieces = position->pieceTables[white][ind];
        uint64_t blackPieces = position->pieceTables[black][ind];
        const evalParameters* evalPars      = getEvalParameters();
        val = val+evalPars->figureValues[ind]*(popcount(whitePieces)-popcount(blackPieces));
    }
    return val;
}


uint16_t calcTotalFigureEvaluation(const chessPosition* position) {
    uint16_t val = 0;
    for (uint16_t ind = 0; ind < 5; ind++) {
        uint64_t whitePieces = position->pieceTables[white][ind];
        uint64_t blackPieces = position->pieceTables[black][ind];
        const evalParameters* evalPars      = getEvalParameters();
        val = val+evalPars->figureValues[ind]*(popcount(whitePieces)+popcount(blackPieces));
    }
    return val;
}

