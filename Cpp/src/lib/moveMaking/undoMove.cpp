/*
 * undoMove.cpp
 *
 *  Created on: Sep 21, 2016
 *      Author: vabi
 */

#include <lib/basics.hpp>
#include <lib/Defines/boardParts.hpp>
#include <iostream>
#include <lib/DebugFunctions/debugFunctions.hpp>
#include <hashTables/hashTables.hpp>
#include <assert.h>
#include <userInterface/UIlayer.hpp>
#include <parameters/parameters.hpp>
#include <lib/Evaluation/PSQ.hpp>

extern uint16_t repetitionData[16384];
extern uint64_t castlingHash[16];
extern uint64_t enpassantHash[9];

void undoNullMove(chessPosition* position) {
    position->zobristHash = position->zobristHash^enpassantHash[position->data.enPassantFile];
    position->data = position->dataStack.pop();
    position->zobristHash = position->zobristHash^getMovingSideHash(white);
    position->toMove = (playerColor) (1-position->toMove);
    position->zobristHash = position->zobristHash^enpassantHash[position->data.enPassantFile];
    position->madeMoves.pop();
}

inline static void undoNormalMove(chessPosition* position, chessMove move) {
    position->toMove = (playerColor) (1-position->toMove);
    playerColor toMove                                  = position->toMove;
    uint64_t moveMask                                   = BIT64(move.sourceField) | BIT64(move.targetField);
    position->pieces[toMove]                            = position->pieces[toMove]^moveMask;
    position->pieceTables[toMove][move.type]            = position->pieceTables[toMove][move.type]^moveMask;
    uint64_t isCapture = ((none == move.captureType) ? 0:UINT64_MAX);
    position->pieces[1-toMove]                          = (position->pieces[1-toMove] | (isCapture & moveMask)) & (~position->pieces[toMove]);
    position->pieceTables[1-toMove][move.captureType]   = (position->pieceTables[1-toMove][move.captureType] | (isCapture & moveMask)) & (~position->pieces[toMove]);
    position->pieceTableEval = position->pieceTableEval-(1-2*toMove)*(getPSQentry(move.type, toMove, move.targetField)-getPSQentry(move.type, toMove, move.sourceField));
    position->pieceTableEval = position->pieceTableEval-(1-2*toMove)*getPSQentry(move.captureType,  INVERTCOLOR(toMove), move.targetField);
    position->zobristHash    = position->zobristHash^getHashEntry((figureType) move.type, toMove, move.targetField)^getHashEntry((figureType) move.type, toMove, move.sourceField)^getHashEntry(move.captureType, INVERTCOLOR(toMove), move.targetField);
    position->pawnHash    = position->pawnHash^getPawnZobristHashEntry((figureType) move.type, toMove, move.targetField)^getPawnZobristHashEntry((figureType) move.type, toMove, move.sourceField)^getPawnZobristHashEntry((figureType) move.captureType, INVERTCOLOR(toMove), move.targetField);
}

inline static void undoKingSideCastling(chessPosition* position) {
    //TODO: remove the ifs
    position->toMove = (playerColor) (1-position->toMove);
    playerColor toMove = position->toMove;

    if (toMove == white) {
        position->pieces[toMove]             = position->pieces[toMove]^(WHITEKINGSIDECASTLEOCCUPANCYCHANGE);
        position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(WHITEKINGSIDECASTLEROOKMOVE);
        position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(WHITEKINGSIDECASTLEKINGMOVE);
        position->pieceTableEval             = position->pieceTableEval-(-getPSQentry(rook, white, 7)+getPSQentry(rook, white, 5)-getPSQentry(king, white, 4)+getPSQentry(king, white, 6));
        position->zobristHash                = position->zobristHash^getHashEntry(rook, white, 7)^getHashEntry(rook, white, 5)^getHashEntry(king, white, 4)^getHashEntry(king, white, 6);
    } else {
        position->pieces[toMove]             = position->pieces[toMove]^(BLACKKINGSIDECASTLEOCCUPANCYCHANGE);
        position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(BLACKKINGSIDECASTLEROOKMOVE);
        position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(BLACKKINGSIDECASTLEKINGMOVE);
        position->pieceTableEval             = position->pieceTableEval-getPSQentry(rook, black, 63)+getPSQentry(rook, black, 61)-getPSQentry(king, black, 60)+getPSQentry(king, black, 62);
        position->zobristHash                = position->zobristHash^getHashEntry(rook, black, 63)^getHashEntry(rook, black, 61)^getHashEntry(king, black, 60)^getHashEntry(king, black, 62);
    }
}

inline static void undoQueenSideCastling(chessPosition* position) {
    position->toMove = (playerColor) (1-position->toMove);
    playerColor toMove = position->toMove;
    if (toMove == white) {
        position->pieces[toMove]             = position->pieces[toMove]^(WHITEQUEENSIDECASTLEOCCUPANCYCHANGE);
        position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(WHITEQUEENSIDECASTLEROOKMOVE);
        position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(WHITEQUEENSIDECASTLEQUEENMOVE);
        position->pieceTableEval             = position->pieceTableEval-(-getPSQentry(rook, white, 0)+getPSQentry(rook, white, 3)-getPSQentry(king, white, 4)+getPSQentry(king, white, 2));
        position->zobristHash                = position->zobristHash^getHashEntry(rook, white, 0)^getHashEntry(rook, white, 3)^getHashEntry(king, white, 4)^getHashEntry(king, white, 2);
    } else {
        position->pieces[toMove]             = position->pieces[toMove]^(BLACKQUEENSIDECASTLEOCCUPANCYCHANGE);
        position->pieceTables[toMove][rook]  = position->pieceTables[toMove][rook]^(BLACKQUEENSIDECASTLEROOKMOVE);
        position->pieceTables[toMove][king]  = position->pieceTables[toMove][king]^(BLACKQUEENSIDECASTLEQUEENMOVE);
        position->pieceTableEval             = position->pieceTableEval-getPSQentry(rook, black, 56)+getPSQentry(rook, black, 59)-getPSQentry(king, black, 60)+getPSQentry(king, black, 58);
        position->zobristHash                = position->zobristHash^getHashEntry(rook, black, 56)^getHashEntry(rook, black, 59)^getHashEntry(king, black, 60)^getHashEntry(king, black, 58);
    }
}

inline static void undoEnPassant(chessPosition* position, chessMove move) {
    position->toMove = (playerColor) (1-position->toMove);
    playerColor toMove                                  = position->toMove;
    uint64_t moveMask                                   = BIT64(move.sourceField) | BIT64(move.targetField);
    position->pieces[toMove]                            = position->pieces[toMove]^moveMask;
    position->pieceTables[toMove][pawn]                 = position->pieceTables[toMove][pawn]^moveMask;
    uint16_t shift                                      = (toMove? move.targetField+8: move.targetField-8);
    position->pieceTables[1-toMove][pawn]               = position->pieceTables[1-toMove][pawn] ^ BIT64(shift);
    position->pieces[1-toMove]                          = position->pieces[1-toMove] ^ BIT64(shift);
    position->pieceTableEval                            = position->pieceTableEval-(1-2*toMove)*(getPSQentry(pawn, toMove, move.targetField)-getPSQentry(pawn, toMove, move.sourceField));
    position->pieceTableEval                            = position->pieceTableEval-(1-2*toMove)*getPSQentry(pawn, INVERTCOLOR(toMove), shift);
    position->zobristHash = position->zobristHash^getHashEntry(pawn, toMove, move.targetField)^getHashEntry(pawn, toMove, move.sourceField)^getHashEntry(pawn, INVERTCOLOR(toMove), shift);
    position->pawnHash = position->pawnHash^getPawnZobristHashEntry(pawn, toMove, move.targetField)^getPawnZobristHashEntry(pawn, toMove, move.sourceField)^getPawnZobristHashEntry(pawn, INVERTCOLOR(toMove), shift);
}

inline static void undoPromotion(chessPosition* position, chessMove move, figureType promotedFigure) {
    position->toMove = (playerColor) (1-position->toMove);
    playerColor toMove                                  = position->toMove;
    uint64_t moveMask                                   = BIT64(move.sourceField) | BIT64(move.targetField);
    position->pieces[toMove]                            = position->pieces[toMove]^moveMask;
    position->pieceTables[toMove][pawn]                 = position->pieceTables[toMove][pawn]^BIT64(move.sourceField);
    position->pieceTables[toMove][promotedFigure]       = position->pieceTables[toMove][promotedFigure]^BIT64(move.targetField);
    uint64_t isCapture                                  = ((none == move.captureType) ? 0:UINT64_MAX);
    position->pieces[1-toMove]                          = (position->pieces[1-toMove] | (isCapture & moveMask)) & (~position->pieces[toMove]);
    position->pieceTables[1-toMove][move.captureType]   = (position->pieceTables[1-toMove][move.captureType] | (isCapture & moveMask)) & (~position->pieces[toMove]);
    position->pieceTableEval                            = position->pieceTableEval-(1-2*toMove)*(getPSQentry(promotedFigure, toMove, move.targetField)-getPSQentry(pawn, toMove, move.sourceField));
    position->pieceTableEval                            = position->pieceTableEval-(1-2*toMove)*getPSQentry(move.captureType, INVERTCOLOR(toMove), move.targetField);
    const evalParameters* evalPars                      = getEvalParameters();
    position->figureEval                                = position->figureEval-(1-2*toMove)*(evalPars->figureValues[promotedFigure]-evalPars->figureValues[pawn]);
    position->totalFigureEval                           = position->totalFigureEval-(evalPars->figureValues[promotedFigure]-evalPars->figureValues[pawn]);
    position->zobristHash = position->zobristHash^getHashEntry(promotedFigure, toMove, move.targetField)^getHashEntry(pawn, toMove, move.sourceField)^getHashEntry(move.captureType, INVERTCOLOR(toMove), move.targetField);
    position->pawnHash = position->pawnHash^getPawnZobristHashEntry(pawn, toMove, move.sourceField)^getPawnZobristHashEntry(move.captureType, INVERTCOLOR(toMove), move.targetField);
}


void undoMove(chessPosition* position) {
    if (position->madeMoves.length == 0) {
        return;
    }

    /*if (repetitionData[position->zobristHash & 16383] == 0) {
        std::cout << position->madeMoves.length << std::endl;
        std::cout << chessPositionToOutputString(*position) << std::endl;
        std::cout << chessPositionToFenString(*position, false) << std::endl;
        for (uint16_t ind = 0; ind < position->madeMoves.length; ind++) {
            std::cout << position->madeMoves[ind].sourceField << " " << position->madeMoves[ind].targetField << std::endl;
        }
        std::cout << "WTF" << std::endl;
    }*/

    assert(repetitionData[position->zobristHash & 16383] != 0);
    repetitionData[position->zobristHash & 16383]--;
    chessMove move                      = position->madeMoves.pop();
    position->zobristHash               = position->zobristHash^enpassantHash[position->data.enPassantFile];
    position->zobristHash               = position->zobristHash^castlingHash[position->data.castlingRights];
    position->data                      = position->dataStack.pop();
    position->zobristHash               = position->zobristHash^castlingHash[position->data.castlingRights];
    position->zobristHash               = position->zobristHash^enpassantHash[position->data.enPassantFile];
    const evalParameters* evalPars      = getEvalParameters();
    position->figureEval                = position->figureEval+(1-2*position->toMove)*evalPars->figureValues[move.captureType];
    position->totalFigureEval           = position->totalFigureEval+evalPars->figureValues[move.captureType];

    //00000BNR00000PKQ00000NPR00000N0P000000000000000p0qpN0npnN000000kw0000

    switch (move.type) {
        case pawnMove:
        case knightMove:
        case bishopMove:
        case rookMove:
        case queenMove:
        case kingMove:
                {
                undoNormalMove(position, move);
                break;
                }
        case castlingKingside:
            {
                undoKingSideCastling(position);
                break;
            }
            case castlingQueenside:
            {
                undoQueenSideCastling(position);
                break;
            }
            case enpassant: {
                undoEnPassant(position, move);
                break;
            }
            case promotionQueen:
            case promotionRook:
            case promotionBishop:
            case promotionKnight:
            {
                undoPromotion(position, move, (figureType) (move.type-8));
                break;
            }
            default:
                break;
    }

    position->zobristHash = position->zobristHash^getMovingSideHash(white);



    assert(position->zobristHash == position->data.hash);

    #ifdef DEBUG

    if (position->zobristHash != position->data.hash) {
        std::cout << chessPositionToOutputString(*position) << std::endl;
        std::cout << "WTF???" << std::endl;
    }
    debug_incremental_calculations(position);

    #endif
}
