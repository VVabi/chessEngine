/*
 * moveMaking.cpp
 *
 *  Created on: Sep 2, 2016
 *      Author: vabi
 */

#include <lib/basics.hpp>
#include <lib/Defines/boardParts.hpp>
#include <iostream>
#include <lib/DebugFunctions/debugFunctions.hpp>
#include <hashTables/hashTables.hpp>
#include <assert.h>
#include <parameters/parameters.hpp>
#include <lib/Defines/figureValues.hpp>
#include <lib/Evaluation/PSQ.hpp>

extern uint64_t movingSideHash[2];
extern uint64_t castlingHash[16];
extern uint64_t enpassantHash[9];
extern uint16_t repetitionData[16384];

static int16_t figureValues[7] = {PAWNVALUE, KNIGHTVALUE, BISHOPVALUE, ROOKVALUE, QUEENVALUE, 10000, 0};

void makeNullMove(chessPosition* position) {
    position->data.hash = position->zobristHash;
    position->dataStack.add(&position->data);
    position->zobristHash = position->zobristHash^enpassantHash[position->data.enPassantFile];
    position->data.enPassantFile  = 8;
    position->zobristHash = position->zobristHash^enpassantHash[position->data.enPassantFile];
    position->data.fiftyMoveRuleCounter = 0;
    position->zobristHash = position->zobristHash^movingSideHash[0];
    position->toMove = (playerColor) (1-position->toMove);
    chessMove move;
    move.sourceField = 0;
    move.targetField = 0;

    if (position->madeMoves.length > 0) {
        assert(!((position->madeMoves[position->madeMoves.length-1].sourceField == 0) && (position->madeMoves[position->madeMoves.length-1].targetField == 0)));
    }
    position->madeMoves.add(&move);
}

static inline void makeNormalMove(chessMove* move, chessPosition* position) {
    playerColor toMove                                  = position->toMove;
    uint64_t moveMask                                   = BIT64(move->sourceField) | BIT64(move->targetField);
    position->pieces[toMove]                            = position->pieces[toMove]^moveMask;
    position->pieces[INVERTCOLOR(toMove)]                          = position->pieces[INVERTCOLOR(toMove)] & (~moveMask);
    position->pieceTables[toMove][move->type]           = position->pieceTables[toMove][move->type]^moveMask;
    position->pieceTables[INVERTCOLOR(toMove)][move->captureType]  = position->pieceTables[INVERTCOLOR(toMove)][move->captureType] & (~moveMask);

    position->pieceTableEval = position->pieceTableEval+(1-2*toMove)*(getPSQentry(move->type, toMove, move->targetField)-getPSQentry(move->type, toMove, move->sourceField));
    position->pieceTableEval = position->pieceTableEval+(1-2*toMove)*getPSQentry(move->captureType, (playerColor) (INVERTCOLOR(toMove)), move->targetField);

    position->zobristHash    = position->zobristHash^getHashEntry((figureType) move->type, toMove, move->targetField)^getHashEntry((figureType) move->type, toMove, move->sourceField)^getHashEntry((figureType) move->captureType, INVERTCOLOR(toMove), move->targetField);
    position->pawnHash       = position->pawnHash^getPawnZobristHashEntry((figureType) move->type, toMove, move->targetField)^getPawnZobristHashEntry((figureType) move->type, toMove, move->sourceField)^getPawnZobristHashEntry(move->captureType, INVERTCOLOR(toMove), move->targetField);
}


//TODO: get rid of the ifs
static inline void makeKingSideCastle(chessPosition* position) {
    playerColor toMove = position->toMove;

    if (toMove == white) {
        position->pieces[toMove]             = position->pieces[toMove]^(WHITEKINGSIDECASTLEOCCUPANCYCHANGE);
        position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(WHITEKINGSIDECASTLEROOKMOVE);
        position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(WHITEKINGSIDECASTLEKINGMOVE);
        position->pieceTableEval             = position->pieceTableEval-getPSQentry(rook, white, 7)+getPSQentry(rook, white, 5)-getPSQentry(king, white, 4)+getPSQentry(king, white, 6);
        position->zobristHash                = position->zobristHash^getHashEntry(rook, white, 7)^getHashEntry(rook, white, 5)^getHashEntry(king, white, 4)^getHashEntry(king, white, 6);

    } else {
        position->pieces[toMove]             = position->pieces[toMove]^(BLACKKINGSIDECASTLEOCCUPANCYCHANGE);
        position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(BLACKKINGSIDECASTLEROOKMOVE);
        position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(BLACKKINGSIDECASTLEKINGMOVE);
        position->pieceTableEval             = position->pieceTableEval-(-getPSQentry(rook, black, 63)+getPSQentry(rook, black, 61)-getPSQentry(king, black, 60)+getPSQentry(king, black, 62));
        position->zobristHash                = position->zobristHash^getHashEntry(rook, black, 63)^getHashEntry(rook, black, 61)^getHashEntry(king, black, 60)^getHashEntry(king, black, 62);
    }
}


static inline void makeQueenSideCastle(chessPosition* position) {
    playerColor toMove = position->toMove;
    if (toMove == white) {
        position->pieces[toMove]             = position->pieces[toMove]^(WHITEQUEENSIDECASTLEOCCUPANCYCHANGE);
        position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(WHITEQUEENSIDECASTLEROOKMOVE);
        position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(WHITEQUEENSIDECASTLEQUEENMOVE);
        position->pieceTableEval             = position->pieceTableEval-getPSQentry(rook, white, 0)+getPSQentry(rook, white, 3)-getPSQentry(king, white, 4)+getPSQentry(king, white, 2);
        position->zobristHash                = position->zobristHash^getHashEntry(rook, white, 0)^getHashEntry(rook, white, 3)^getHashEntry(king, white, 4)^getHashEntry(king, white, 2);
    } else {
        position->pieces[toMove]             = position->pieces[toMove]^(BLACKQUEENSIDECASTLEOCCUPANCYCHANGE);
        position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(BLACKQUEENSIDECASTLEROOKMOVE);
        position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(BLACKQUEENSIDECASTLEQUEENMOVE);
        position->pieceTableEval             = position->pieceTableEval-(-getPSQentry(rook, black, 56)+getPSQentry(rook, black, 59)-getPSQentry(king, black, 60)+getPSQentry(king, black, 58));
        position->zobristHash                = position->zobristHash^getHashEntry(rook, black, 56)^getHashEntry(rook, black, 59)^getHashEntry(king, black, 60)^getHashEntry(king, black, 58);
    }
}

static inline void makeEnPassant(chessMove* move, chessPosition* position) {
    playerColor toMove                                  = position->toMove;
    uint64_t moveMask                                   = BIT64(move->sourceField) | BIT64(move->targetField);
    position->pieces[toMove]                            = position->pieces[toMove]^moveMask;
    position->pieceTables[toMove][pawn]                 = position->pieceTables[toMove][pawn]^moveMask;
    uint16_t shift                                      = (toMove? move->targetField+8: move->targetField-8);
    position->pieceTables[INVERTCOLOR(toMove)][move->captureType]  = position->pieceTables[INVERTCOLOR(toMove)][pawn] ^ BIT64(shift);
    position->pieces[INVERTCOLOR(toMove)]                          = position->pieces[INVERTCOLOR(toMove)] ^ BIT64(shift);
    position->pieceTableEval = position->pieceTableEval+(1-2*toMove)*(getPSQentry(pawn, toMove, move->targetField)-getPSQentry(pawn, toMove, move->sourceField));
    position->pieceTableEval = position->pieceTableEval+(1-2*toMove)*getPSQentry(pawn, INVERTCOLOR(toMove), shift);

    position->zobristHash = position->zobristHash^getHashEntry(pawn, toMove, move->targetField)^getHashEntry(pawn, toMove, move->sourceField)^getHashEntry(pawn, INVERTCOLOR(toMove), shift);
    position->pawnHash = position->pawnHash^getPawnZobristHashEntry(pawn, toMove, move->targetField)^getPawnZobristHashEntry(pawn, toMove, move->sourceField)^getPawnZobristHashEntry(pawn, INVERTCOLOR(toMove), shift);
    //position->totalFigureEval     = position->totalFigureEval-figureValues[pawn];
}


static inline void makePromotion(chessMove* move, chessPosition* position, figureType promotedFigure) {
    playerColor toMove                                  = position->toMove;
    uint64_t moveMask                                   = BIT64(move->sourceField) | BIT64(move->targetField);
    position->pieces[toMove]                            = position->pieces[toMove]^moveMask;
    position->pieces[INVERTCOLOR(toMove)]                          = position->pieces[INVERTCOLOR(toMove)] & (~moveMask);
    position->pieceTables[toMove][pawn]                 = position->pieceTables[toMove][pawn]  ^ BIT64(move->sourceField);
    position->pieceTables[toMove][promotedFigure]       = position->pieceTables[toMove][promotedFigure] ^ BIT64(move->targetField);
    position->pieceTables[INVERTCOLOR(toMove)][move->captureType]  = position->pieceTables[INVERTCOLOR(toMove)][move->captureType] & (~moveMask);

    position->pieceTableEval = position->pieceTableEval+(1-2*toMove)*(getPSQentry(promotedFigure, toMove, move->targetField)-getPSQentry(pawn, toMove, move->sourceField));
    position->pieceTableEval = position->pieceTableEval+(1-2*toMove)*getPSQentry(move->captureType, (playerColor) (INVERTCOLOR(toMove)), move->targetField);
    position->zobristHash    = position->zobristHash^getHashEntry(promotedFigure, toMove, move->targetField)^getHashEntry(pawn, toMove, move->sourceField)^getHashEntry(move->captureType, INVERTCOLOR(toMove), move->targetField);
    position->pawnHash       = position->pawnHash^getPawnZobristHashEntry(pawn, toMove, move->sourceField)^getPawnZobristHashEntry(move->captureType, INVERTCOLOR(toMove), move->targetField);
    //const evalParameters* evalPars = getEvalParameters();
    position->figureEval     = position->figureEval+(1-2*toMove)*(figureValues[promotedFigure]-figureValues[pawn]);
    position->totalFigureEval     = position->totalFigureEval+(figureValues[promotedFigure]-figureValues[pawn]);
}

static uint64_t movemakecalls = 0;

void makeMove(chessMove* move, chessPosition* position) {
    movemakecalls++;
    assert(move->captureType != king);
    position->data.hash = position->zobristHash;
    position->dataStack.add(&position->data);
    position->data.fiftyMoveRuleCounter = position->data.fiftyMoveRuleCounter+1;

    bool isPawn = (move->type == pawnMove) || (move->type == enpassant) || (move->type == promotionQueen) || (move->type == promotionRook) || (move->type == promotionBishop) || (move->type == promotionKnight);
    bool isCapture = (move->captureType != none);

    if (isPawn || isCapture) {
        position->data.fiftyMoveRuleCounter = 0;
    }

    uint8_t castlingRights = position->data.castlingRights;
    position->zobristHash = position->zobristHash^castlingHash[position->data.castlingRights];
    uint64_t moveMask      = BIT64(move->sourceField) | BIT64(move->targetField);
    castlingRights = (moveMask & WHITEKINGSIDECASTLEMASK  ? (castlingRights & 14):castlingRights);
    castlingRights = (moveMask & WHITEQUEENSIDECASTLEMASK ? (castlingRights & 13):castlingRights);
    castlingRights = (moveMask & BLACKKINGSIDECASTLEMASK  ? (castlingRights & 11):castlingRights);
    castlingRights = (moveMask & BLACKQUEENSIDECASTLEMASK ? (castlingRights &  7):castlingRights);
    position->data.castlingRights = castlingRights;
    position->zobristHash = position->zobristHash^castlingHash[position->data.castlingRights];
    //const evalParameters* evalPars = getEvalParameters();
    position->figureEval     = position->figureEval+(1-2*position->toMove)*figureValues[move->captureType];
    position->totalFigureEval     = position->totalFigureEval-figureValues[move->captureType];
    position->zobristHash = position->zobristHash^enpassantHash[position->data.enPassantFile];
    position->data.enPassantFile  = 8;

    if ((move->type == pawnMove) && ((move->targetField-move->sourceField) & 15) == 0) { //pawn went two ahead
        position->data.enPassantFile = FILE(move->targetField);
    }


    switch (move->type) {
        case pawnMove:
        case knightMove:
        case bishopMove:
        case rookMove:
        case queenMove:
        case kingMove:
        {
            makeNormalMove(move, position);
            break;
        }
        case castlingKingside:
        {
            makeKingSideCastle(position);
            break;
        }
        case castlingQueenside:
        {
            makeQueenSideCastle(position);
            break;
        }
        case enpassant:
        {
            makeEnPassant(move, position);
            break;
        }
        case promotionQueen:
        case promotionRook:
        case promotionBishop:
        case promotionKnight:
        {
            makePromotion(move, position, (figureType) (move->type-8));
            break;
        }
        default:
            break;
    }
    position->zobristHash = position->zobristHash^enpassantHash[position->data.enPassantFile];
    position->madeMoves.add(move);
    position->toMove = INVERTCOLOR(position->toMove);
    position->zobristHash = position->zobristHash^movingSideHash[0];
    repetitionData[position->zobristHash & 16383]++;

    #ifdef DEBUG
    debug_incremental_calculations(position);
    #endif
}



