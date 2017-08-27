/*
 * basics.cpp
 *
 *  Created on: Sep 1, 2016
 *      Author: vabi
 */


#include "basics.hpp"
#include <iostream>
#include <string.h>
#include <lib/bitfiddling.h>
#include <lib/Defines/boardParts.hpp>
#include <hashTables/hashTables.hpp>
#include <lib/DebugFunctions/debugFunctions.hpp>
#include <userInterface/UIlayer.hpp>


inline figureType toFigureType(uint16_t num) {
#ifdef DEBUG
    if (num > 5) {
        std::cout << "There are only 6 different figuretypes in chess..." << std::endl;
        while (1) {}
    }
#endif
    return (figureType) num;
}



void zeroInitPosition(chessPosition* position) {
    memset(position, 0, sizeof(chessPosition));
    position->madeMoves = vdt_vector<chessMove>(600);
    position->dataStack = vdt_vector<pathDependentPositionData>(600);
    position->zobristHash    = calcZobristHash(position);
    position->data.castlingRights = 0;
    position->data.fiftyMoveRuleCounter  = 0;
    position->data.enPassantFile = 8;
    position->presentPieces.reset();
}




void debug_incremental_calculations(const chessPosition* position) {
    int16_t eval = calcFigureEvaluation(position);

    /*if (eval != position->figureEval) {
        std::cout << "Figure evaluation is wrong after moveMake!" << std::endl;
    }*/

    uint16_t totalEval = calcTotalFigureEvaluation(position);

    if (totalEval != position->totalFigureEval) {
        logError("Total figure evaluation is wrong after moveMake!" +chessPositionToFenString(*position));
    }

    int16_t pieceTableEval = calcPieceTableValue(position)+eval;
    int32_t buffer =  position->pieceTableEval & 0xFFFF;
    int32_t buffer2  = buffer-(1 << 15);

    if (pieceTableEval !=  buffer2) {
        logError("Piece table evaluation is wrong after moveMake!");
    }

    int16_t endgamepieceTableEval = calcEndGamePieceTableValue(position)+eval;
    buffer =  (position->pieceTableEval >> 16);
    buffer2  = buffer-(1 << 14);
    if (endgamepieceTableEval !=  buffer2) {
        logError("endgame Piece table evaluation is wrong after moveMake!");
    }

    uint64_t hash = calcZobristHash(position);
    if (hash != position->zobristHash) {
        logError("zobrist hash wrong after make move");
    }

    uint64_t pawnhash = calcPawnHash(position);
    if (pawnhash != position->pawnHash) {
        logError("zobrist pawn hash wrong after make move");
    }

    PresentPieces p = calcPresentPieces(position);

    if (p.getInternalValue() != position->presentPieces.getInternalValue()) {
        logError("Present pieces table is wrong");
    }
    const evalParameters* evalPars      = getEvalParameters();
    uint32_t totalFigureEval = 0;
    uint64_t val = position->presentPieces.getInternalValue();
    for (uint16_t side=0; side < 2; side++) {
        for (uint16_t figure=0; figure < 5; figure++) {
            uint16_t offset = 32*side+4*figure;
            uint64_t mask = BIT64(offset) | BIT64(offset+1) | BIT64(offset+2) | BIT64(offset+3);
            uint64_t pieces = mask & val;
            totalFigureEval += (pieces >> offset)*evalPars->figureValues[figure];
        }
    }

    if (totalFigureEval != position->totalFigureEval) {
        logError("Present pieces table disagrees with total figures eval??");
    }
}

void free_position(chessPosition* position) {
    position->dataStack.free_array();
    position->madeMoves.free_array();
}

