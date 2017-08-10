/*
 * pieceTableCalc.cpp
 *
 *  Created on: Sep 19, 2016
 *      Author: vabi
 */

#include <lib/basics.hpp>
#include <lib/bitfiddling.h>
#include <lib/Evaluation/PSQ.hpp>

int16_t calcPieceTableValue(const chessPosition* position) {
    int16_t val = 0;
    for (uint16_t ind = 0; ind < 6; ind++) {
        uint64_t whitePieces = position->pieceTables[white][ind];
        while (whitePieces) {
            uint16_t field = popLSB(whitePieces);
            val = val+getEarlyGamePSQentry((figureType) ind, white, field);;
        }
        uint64_t blackPieces = position->pieceTables[black][ind];
        while (blackPieces) {
            uint16_t field = popLSB(blackPieces);
            val = val-getEarlyGamePSQentry((figureType) ind, black, field);
        }
    }
    return val;
}


int16_t calcEndGamePieceTableValue(const chessPosition* position) {
    int16_t val = 0;
    for (uint16_t ind = 0; ind < 6; ind++) {
        uint64_t whitePieces = position->pieceTables[white][ind];
        while (whitePieces) {
            uint16_t field = popLSB(whitePieces);
            val = val+getEndgameGamePSQentry((figureType) ind, white, field);
        }
        uint64_t blackPieces = position->pieceTables[black][ind];
        while (blackPieces) {
            uint16_t field = popLSB(blackPieces);
            val = val-getEndgameGamePSQentry((figureType) ind, black, field);
        }
    }
    return val;
}


