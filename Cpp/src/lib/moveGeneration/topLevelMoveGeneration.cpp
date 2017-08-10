/*
 * topLevelMoveGeneration.cpp
 *
 *  Created on: Sep 1, 2016
 *      Author: vabi
 */


#include <DataTypes/vdt_vector.hpp>
#include <lib/basics.hpp>
#include <lib/Defines/knightmoves.hpp>
#include <lib/Defines/kingMoves.hpp>
#include <lib/Defines/boardParts.hpp>
#include <lib/bitfiddling.h>
#include "moveGenerationInternals.hpp"

//TODO: for debug purposes I want these rangechecked!
extern uint64_t knightmovetables[];
extern uint64_t kingmovetables[];

extern uint64_t castlingBlockers[2][2];
extern uint64_t enPassantMoveTable[2][8];
extern uint16_t enPassantTargetFields[2][8];

static uint64_t promotionRows[] = {LASTROW, FIRSTROW};

__attribute__((always_inline)) static inline void extractMoves(const uint64_t currentPiece, const figureType figure, uint64_t potentialMoves, vdt_vector<chessMove>* vec, chessPosition* position) {
    uint16_t sourceField = findLSB(currentPiece);
    playerColor toMove = position->toMove;
    while (potentialMoves != 0) {
        uint64_t nextMove = LOWESTBITONLY(potentialMoves);
        figureType captureType = none;
        if (nextMove & position->pieces[1-toMove]) {
            for (uint16_t ind = 0; ind < NUM_DIFFERENT_PIECES; ind++) {
                if (nextMove & position->pieceTables[1-toMove][ind]) {
                    captureType = (figureType) ind;
                    break;
                }
            }
        }
        chessMove move;
        move.type = (moveType) figure;
        move.sourceField = sourceField;
        move.targetField = findLSB(nextMove);
        move.captureType = captureType;
        vec->add(&move);
        potentialMoves = potentialMoves & (~nextMove);
    }
}

__attribute__((always_inline)) static inline void generateRookMoves(vdt_vector<chessMove>* vec, chessPosition* position, const figureType figure, const uint64_t targetMask) {
    playerColor toMove = position->toMove;
    uint64_t pieces    = position->pieceTables[toMove][figure];
    uint64_t occupancy  = position->pieces[white] | position->pieces[black];
    while (pieces != 0) {
        uint64_t nextPiece = LOWESTBITONLY(pieces);
        uint16_t nextPieceField = popLSB(pieces);
        uint64_t potentialMoves = getPotentialRookMoves(nextPieceField, occupancy);
        potentialMoves = potentialMoves & (~position->pieces[toMove]) & targetMask;
        extractMoves(nextPiece, figure, potentialMoves, vec, position);
        pieces = pieces & (~nextPiece);
    }


}

__attribute__((always_inline)) static inline void generateBishopMoves(vdt_vector<chessMove>* vec, chessPosition* position, const figureType figure, const uint64_t targetMask) {
    playerColor toMove = position->toMove;
    uint64_t pieces    = position->pieceTables[toMove][figure];
    uint64_t occupancy  = position->pieces[white] | position->pieces[black];
    while (pieces != 0) {
        uint64_t nextPiece = LOWESTBITONLY(pieces);
        uint16_t nextPieceField = popLSB(pieces);
        uint64_t potentialMoves = getPotentialBishopMoves(nextPieceField, occupancy);
        potentialMoves = potentialMoves & (~position->pieces[toMove]) & targetMask;
        extractMoves(nextPiece, figure, potentialMoves, vec, position);
        pieces = pieces & (~nextPiece);
    }
}

__attribute__((always_inline)) static inline void generateNonSliderMoves(vdt_vector<chessMove>* vec, chessPosition* position, const uint64_t* moveTable, const figureType figure, const uint64_t targetMask) {
    playerColor toMove = position->toMove;
    uint64_t pieces    = position->pieceTables[toMove][figure];
    while (pieces != 0) {
        uint64_t nextPiece = LOWESTBITONLY(pieces);
        uint16_t nextPieceField = popLSB(pieces);
        uint64_t potentialMoves = moveTable[nextPieceField];
        potentialMoves = potentialMoves & (~position->pieces[toMove]) & targetMask;
        extractMoves(nextPiece, figure, potentialMoves, vec, position);
        pieces = pieces & (~nextPiece);
    }
}


__attribute__((always_inline)) static inline void generatePawnMoves(vdt_vector<chessMove>* vec, chessPosition* position, uint64_t mask) {
    //TODO: generate promotions! And this function is too complicated
    playerColor toMove = position->toMove;
    uint64_t occupancy  = position->pieces[white] | position->pieces[black];
    uint64_t pawns     = position->pieceTables[toMove][pawn];
    uint64_t forward   = (toMove? pawns >> 8: pawns << 8);
    forward            = forward & (~occupancy) & mask;

    while (forward != 0)  {
        uint64_t target = LOWESTBITONLY(forward);
        uint64_t source = (toMove? target << 8: target >> 8);
        chessMove move;
        move.type = pawnMove;
        move.captureType = none;
        move.sourceField = findLSB(source);
        move.targetField = findLSB(target);
        uint64_t promotionRow = promotionRows[position->toMove];
        if (target & promotionRow) {
            move.type = promotionQueen;
            vec->add(&move);
            move.type = promotionRook;
            vec->add(&move);
            move.type = promotionBishop;
            vec->add(&move);
            move.type = promotionKnight;
            vec->add(&move);
        } else {
            vec->add(&move);
        }
        forward = forward & (~target);
    }

    uint64_t homerow            = (toMove ? SEVENTHROW : SECONDROW);
    uint64_t onHomeRow          = pawns & homerow;
    forward                     = (toMove?  onHomeRow >> 16:  onHomeRow << 16);
    uint64_t occupancyShifted   = (toMove?  occupancy >> 8:  occupancy << 8);
    forward                     = forward & (~occupancy) & (~occupancyShifted) & mask;

    while (forward != 0) {
        uint64_t target = LOWESTBITONLY(forward);
        uint64_t source = (toMove? target << 16: target >> 16);
        chessMove move;
        move.type = pawnMove;
        move.captureType = none;
        move.sourceField = findLSB(source);
        move.targetField = findLSB(target);
        vec->add(&move);
        forward = forward & (~target);
    }

    uint64_t takesLeft = (toMove? pawns >> 9 : pawns << 7) & NOTFILEH & position->pieces[1-toMove] & mask;

    while (takesLeft) {
        uint64_t target     = LOWESTBITONLY(takesLeft);
        uint64_t source     = (toMove? target << 9: target >> 7);
        figureType captureType = none;
        for (uint16_t ind = 0; ind < NUM_DIFFERENT_PIECES; ind++) {
            if (target & position->pieceTables[1-toMove][ind]) {
                captureType = (figureType) ind;
                break;
            }
        }
        #ifdef DEBUG
        if (captureType == none) {
            std::cout << "Pawn capture move generation is buggy" << std::endl;

        }
        #endif
        chessMove move;
        move.type = pawnMove;
        move.captureType = captureType;
        move.sourceField = findLSB(source);
        move.targetField = findLSB(target);
        uint64_t promotionRow = promotionRows[position->toMove];
        if (target & promotionRow) {
            move.type = promotionQueen;
            vec->add(&move);
            move.type = promotionRook;
            vec->add(&move);
            move.type = promotionBishop;
            vec->add(&move);
            move.type = promotionKnight;
            vec->add(&move);
        } else {
            vec->add(&move);
        }
        takesLeft           = takesLeft & (~target);
    }

    uint64_t takesRight = (toMove? pawns >> 7 : pawns << 9) & NOTFILEA & position->pieces[1-toMove]& mask;

    while (takesRight) {
        uint64_t target     = LOWESTBITONLY(takesRight);
        uint64_t source     = (toMove? target << 7: target >> 9);
        figureType captureType = none;
        for (uint16_t ind = 0; ind < NUM_DIFFERENT_PIECES; ind++) {
            if (target & position->pieceTables[1-toMove][ind]) {
                captureType = (figureType) ind;
                break;
            }
        }
        #ifdef DEBUG
        if (captureType == none) {
            std::cout << "Pawn capture move generation is buggy" << std::endl;
        }
        #endif
        chessMove move;
        move.type = pawnMove;
        move.captureType = captureType;
        move.sourceField = findLSB(source);
        move.targetField = findLSB(target);
        uint64_t promotionRow = promotionRows[position->toMove];
        if (target & promotionRow) {
            move.type = promotionQueen;
            vec->add(&move);
            move.type = promotionRook;
            vec->add(&move);
            move.type = promotionBishop;
            vec->add(&move);
            move.type = promotionKnight;
            vec->add(&move);
        } else {
            vec->add(&move);
        }
        takesRight          = takesRight & (~target);
    }
}

#define KINGSIDE  0
#define QUEENSIDE 1



__attribute__((always_inline)) static inline void generateCastling(vdt_vector<chessMove>* vec, chessPosition* position) {
    playerColor toMove = position->toMove;
    uint8_t castlingMask = (toMove? 12: 3);
    if (position->data.castlingRights & castlingMask) {
        uint64_t occupancy = (position->pieces[white]) | (position->pieces[black]);


        uint8_t castlingOffset = (toMove? 2: 0);

        if ((position->data.castlingRights & (1 << castlingOffset)) && ((occupancy & castlingBlockers[toMove][KINGSIDE]) == 0)) {
            chessMove mv;
            mv.captureType = none;
            mv.type        = castlingKingside;
            mv.sourceField = (toMove ? 60: 4);
            mv.targetField = (toMove ? 63: 7);
            vec->add(&mv);
        }

        if ((position->data.castlingRights & (1 << (castlingOffset+1))) && ((occupancy & castlingBlockers[toMove][QUEENSIDE]) == 0)) {
            chessMove mv;
            mv.captureType = none;
            mv.type        = castlingQueenside;
            mv.sourceField = (toMove ? 60: 4);
            mv.targetField = (toMove ? 56: 0);
            vec->add(&mv);
        }
    }
}

__attribute__((always_inline)) static inline void generateEnPassant(vdt_vector<chessMove>* vec, chessPosition* position) {

    if (position->data.enPassantFile > 7) {
        return;
    }

    uint64_t mask = (enPassantMoveTable[position->toMove][position->data.enPassantFile]) & (position->pieceTables[position->toMove][pawn]);

    while (mask) {
        uint16_t source = popLSB(mask);
        uint16_t target = enPassantTargetFields[position->toMove][position->data.enPassantFile];

        chessMove mov;
        mov.sourceField = source;
        mov.targetField = target;
        mov.captureType = pawn;
        mov.type        = enpassant;
        vec->add(&mov);
    }
}


void generateAllMoves(vdt_vector<chessMove>* vec, chessPosition* position) {
    generateCastling(vec, position);
    generateNonSliderMoves(vec, position, knightmovetables, knight, UINT64_MAX);
    generateNonSliderMoves(vec, position, kingmovetables, king, UINT64_MAX);
    generateRookMoves(vec, position, rook, UINT64_MAX);
    generateRookMoves(vec, position, queen, UINT64_MAX);
    generateBishopMoves(vec, position, bishop, UINT64_MAX);
    generateBishopMoves(vec, position, queen, UINT64_MAX);
    generatePawnMoves(vec, position, UINT64_MAX);
    generateEnPassant(vec, position);
}

void generateChecks(vdt_vector<chessMove>* vec, chessPosition* position) {
    //TODO: this does currently NOT generate discovered checks, and NO pawn checks
    //note this does NOT generate captures
    playerColor toMove = position->toMove;
    uint64_t nonCaptures = ~position->pieces[1-toMove];
    uint64_t oppKing = position->pieceTables[1-toMove][king];
    uint16_t oppKingField = findLSB(oppKing);
    uint64_t knightChecks = knightmovetables[oppKingField];
    generateNonSliderMoves(vec, position, knightmovetables, knight, knightChecks & nonCaptures);
    uint64_t occupancy = position->pieces[white] | position->pieces[black];
    uint64_t rookChecks = getPotentialRookMoves(oppKingField, occupancy);
    generateRookMoves(vec, position, rook, rookChecks & nonCaptures);
    generateRookMoves(vec, position, queen, rookChecks & nonCaptures);
    uint64_t bishopChecks = getPotentialBishopMoves(oppKingField, occupancy);
    generateBishopMoves(vec, position, bishop, bishopChecks & nonCaptures);
    generateBishopMoves(vec, position, queen, bishopChecks & nonCaptures);
}

void generateAllCaptureMoves(vdt_vector<chessMove>* vec, chessPosition* position) {
    playerColor toMove = position->toMove;
    uint64_t captures = position->pieces[1-toMove];
    generateNonSliderMoves(vec, position, knightmovetables, knight, captures);
    generateNonSliderMoves(vec, position, kingmovetables, king, captures);
    generateRookMoves(vec, position, rook, captures);
    generateRookMoves(vec, position, queen, captures);
    generateBishopMoves(vec, position, bishop, captures);
    generateBishopMoves(vec, position, queen, captures);
    generatePawnMoves(vec, position, captures);
    generateEnPassant(vec, position);
}

