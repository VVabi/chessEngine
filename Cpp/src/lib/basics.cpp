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

    if(num > 5) {
        std::cout << "There are only 6 different figuretypes in chess..." << std::endl;
        while(1) {

        }

    }
#endif

    return (figureType) num;

}



void zeroInitPosition(chessPosition* position) {
#ifdef DEBUGAAA
    position->toMove = white;
    position->pieces = vdt_vector<uint64_t>(2);
    uint64_t dummy = 0;
    position->pieces.add(&dummy);
    position->pieces.add(&dummy);

    vdt_vector<uint64_t> whiteTable = vdt_vector<uint64_t>(NUM_DIFFERENT_PIECES+1);
    whiteTable.add(&dummy);
    whiteTable.add(&dummy);
    whiteTable.add(&dummy);
    whiteTable.add(&dummy);
    whiteTable.add(&dummy);
    whiteTable.add(&dummy);
    whiteTable.add(&dummy);

    vdt_vector<uint64_t> blackTable = vdt_vector<uint64_t>(NUM_DIFFERENT_PIECES+1);
    blackTable.add(&dummy);
    blackTable.add(&dummy);
    blackTable.add(&dummy);
    blackTable.add(&dummy);
    blackTable.add(&dummy);
    blackTable.add(&dummy);
    blackTable.add(&dummy);

    position->pieceTables =  vdt_vector<vdt_vector<uint64_t>>(2);
    position->pieceTables.add(&whiteTable);
    position->pieceTables.add(&blackTable);

    position->enPassantFile = 0;
    position->madeMoves = vdt_vector<chessMove>(300);
    position->castlingAndEpStack = vdt_vector<uint16_t>(300);
    position->figureEval    = 0;
#else
    memset(position, 0, sizeof(chessPosition));
    position->madeMoves = vdt_vector<chessMove>(600);
    position->dataStack = vdt_vector<pathDependentPositionData>(600);
#endif
    position->zobristHash    = calcZobristHash(position);
    position->data.castlingRights = 0;
    position->data.fiftyMoveRuleCounter  = 0;
    position->data.enPassantFile = 8;
}




void debug_incremental_calculations(const chessPosition* position) {

    int16_t eval = calcFigureEvaluation(position);

    /*if(eval != position->figureEval) {
        std::cout << "Figure evaluation is wrong after moveMake!" << std::endl;
    }*/

    uint16_t totalEval = calcTotalFigureEvaluation(position);

    if(totalEval != position->totalFigureEval) {
        logError( "Total figure evaluation is wrong after moveMake!" +chessPositionToFenString(*position));
    }

    int16_t pieceTableEval = calcPieceTableValue(position)+eval;
    int32_t buffer =  position->pieceTableEval & 0xFFFF;
    int32_t buffer2  = buffer-(1 << 15);

    if(pieceTableEval !=  buffer2) {
        logError("Piece table evaluation is wrong after moveMake!");
    }

    int16_t endgamepieceTableEval = calcEndGamePieceTableValue(position)+eval;
    buffer =  (position->pieceTableEval >> 16);
    buffer2  = buffer-(1 << 14);
    if(endgamepieceTableEval !=  buffer2) {
        logError("endgame Piece table evaluation is wrong after moveMake!");
    }

    uint64_t hash = calcZobristHash(position);
    if(hash != position->zobristHash) {
        logError("zobrist hash wrong after make move");
    }

    uint64_t pawnhash = calcPawnHash(position);
    if(pawnhash != position->pawnHash) {
        logError("zobrist pawn hash wrong after make move");
    }

}

void free_position(chessPosition* position) {
    position->dataStack.free_array();
    position->madeMoves.free_array();
}

