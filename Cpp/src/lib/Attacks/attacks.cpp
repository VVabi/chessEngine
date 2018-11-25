/*
 * attacks.cpp
 *
 *  Created on: Sep 19, 2016
 *      Author: vabi
 */


#include <lib/basics.hpp>
#include <lib/bitfiddling.h>
#include <lib/Defines/boardParts.hpp>
#include <lib/moveMaking/moveMaking.hpp>
#include "attacks.hpp"
#include <assert.h>
#include <lib/Evaluation/evaluation.hpp>
#include <parameters/parameters.hpp>
#include <lib/moveGeneration/moveGenerationInternals.hpp>
#include <lib/moveGeneration/nonSliderMoveTables.hpp>

bool isFieldAttacked(const chessPosition* position, playerColor attackingSide, uint16_t field) {
    //knights
    uint64_t knights = position->pieceTables[attackingSide][knight];
    if (getKnightMoves(field) & knights) {
        return true;
    }

    //kings
    uint64_t kings = position->pieceTables[attackingSide][king];
    if (getKingMoves(field) & kings) {
        return true;
    }

    uint64_t occupancy = (position->pieces[white]) | (position->pieces[black]);
    //rooks+queens
    uint64_t potentialMoves = getPotentialRookMoves(field, occupancy);

    if (potentialMoves & (position->pieceTables[attackingSide][rook] | position->pieceTables[attackingSide][queen])) {
        return true;
    }

    //bishops+queens

    potentialMoves = getPotentialBishopMoves(field, occupancy);

    if (potentialMoves & (position->pieceTables[attackingSide][bishop] | position->pieceTables[attackingSide][queen])) {
        return true;
    }

    //pawns
    uint64_t potentialTakes;
    if (attackingSide == white) {
        potentialTakes = ((BIT64(field) >> 9) & NOTFILEH)| ((BIT64(field) >> 7) & NOTFILEA);
    } else {
        potentialTakes = ((BIT64(field) << 9) & NOTFILEA) | ((BIT64(field) << 7) & NOTFILEH);
    }

    if (potentialTakes & position->pieceTables[attackingSide][pawn]) {
        return true;
    }


    return false;
}


AttackTable makeAttackTable(const chessPosition* position, playerColor attackingSide, uint64_t ignoreMask) {
    AttackTable retTable;
    //pawns
    uint64_t pawns = position->pieceTables[attackingSide][pawn];

    uint64_t takesRight = (attackingSide ? pawns >> 7 : pawns << 9) & NOTFILEA;
    uint64_t takesLeft = (attackingSide ? pawns >> 9 : pawns << 7) & NOTFILEH;
    retTable.attackTables[pawn] = takesLeft | takesRight;

    //knights
    uint64_t knights = position->pieceTables[attackingSide][knight];
    uint64_t knightAttackTable = 0;
    while (knights) {
        uint16_t nextKnight = popLSB(knights);
        knightAttackTable = knightAttackTable |getKnightMoves(nextKnight);
    }
    retTable.attackTables[knight] = knightAttackTable;

    //bishops
    uint64_t occupancy  = position->pieces[white] | position->pieces[black];
    occupancy = occupancy &(~ignoreMask);
    uint64_t bishops = position->pieceTables[attackingSide][bishop];
    uint64_t bishopAttackTable = 0;
    while (bishops != 0) {
        uint16_t nextPieceField = popLSB(bishops);
        uint64_t potentialMoves = getPotentialBishopMoves(nextPieceField, occupancy);
        bishopAttackTable = bishopAttackTable | potentialMoves;
    }
    retTable.attackTables[bishop] = bishopAttackTable;
    //rooks

    uint64_t rooks = position->pieceTables[attackingSide][rook];
    uint64_t rookAttackTable = 0;
    while (rooks != 0) {
        uint16_t nextPieceField = popLSB(rooks);
        uint64_t potentialMoves = getPotentialRookMoves(nextPieceField, occupancy);
        rookAttackTable = rookAttackTable | potentialMoves;
    }
    retTable.attackTables[rook] = rookAttackTable;

    //queens


    uint64_t queens = position->pieceTables[attackingSide][queen];
    uint64_t queenAttackTable = 0;
    while (queens != 0) {
        uint16_t nextPieceField = popLSB(queens);
        uint64_t potentialMoves = getPotentialRookMoves(nextPieceField, occupancy);
        queenAttackTable = queenAttackTable | potentialMoves;
    }

    //TODO: this could be optimized by merging with the rook table and doing both at once
    queens = position->pieceTables[attackingSide][queen];
    while (queens != 0) {
        uint16_t nextPieceField = popLSB(queens);
        uint64_t potentialMoves =  getPotentialBishopMoves(nextPieceField, occupancy);
        queenAttackTable = queenAttackTable | potentialMoves;
    }
    retTable.attackTables[queen] = queenAttackTable;


    //kings
    uint64_t kingField = position->pieceTables[attackingSide][king];
    uint64_t kingAttackTable = 0;
    while (kingField) {
        uint16_t nextKing = popLSB(kingField);
        kingAttackTable = kingAttackTable | getKingMoves(nextKing);
    }
    retTable.attackTables[king] = kingAttackTable;
    retTable.completeAttackTable = retTable.attackTables[king] | retTable.attackTables[pawn] | retTable.attackTables[knight] | retTable.attackTables[bishop] | retTable.attackTables[rook] | retTable.attackTables[queen];
    return retTable;
}

/*#ifdef EXPERIMENTAL
int16_t bishopMobility[14]   = {-25, -18, -13, -8, -4, 0, 3, 5, 8, 10, 12, 13, 14, 15};
int16_t rookMobility[15]     = {-25, -19, -14, -10, -7, -5, -2, 0, 2, 4, 6, 8, 10, 12, 13};
int16_t knightMobility[9]   = {-20, -12, -6, 0, 3, 5, 8, 10, 12};
#else*/
int16_t bishopMobility[14]   = {-35, -25, -18, -13, -5, 0, 3, 5, 8, 10, 12, 14, 16, 18};
int16_t rookMobility[15]     = {-35, -25, -18, -15, -10, -8, -3, 0, 3, 5, 8, 10, 12, 14, 16};
int16_t knightMobility[9]   = {-25, -18, -8, 0, 3, 5, 8, 10, 12};
//#endif

AttackTable makeAttackTableWithMobility(const chessPosition* position, playerColor attackingSide, EvalComponentResult* result) {
    uint64_t ownPieces = position->pieces[attackingSide];
    AttackTable retTable;
    //pawns
    uint64_t pawns = position->pieceTables[attackingSide][pawn];

    uint64_t takesRight = (attackingSide ? pawns >> 7 : pawns << 9) & NOTFILEA;
    uint64_t takesLeft = (attackingSide ? pawns >> 9 : pawns << 7) & NOTFILEH;
    retTable.attackTables[pawn] = takesLeft | takesRight;

    uint64_t oppPawns = position->pieceTables[1-attackingSide][pawn];
    uint64_t oppPawnsForward   = attackingSide ? NORTHONE(oppPawns) : SOUTHONE(oppPawns);
    uint64_t opppawnTakes = WESTONE(oppPawnsForward) | EASTONE(oppPawnsForward);


    //knights
    uint64_t knights = position->pieceTables[attackingSide][knight];
    uint64_t knightAttackTable = 0;
    while (knights) {
        uint16_t nextKnight = popLSB(knights);
        knightAttackTable = knightAttackTable | getKnightMoves(nextKnight);
        uint16_t legalMoves = popcount(getKnightMoves(nextKnight) & ~ownPieces & ~opppawnTakes);
        assert(legalMoves < 9);
/*#ifdef EXPERIMENTAL
        result->common     += knightMobility[legalMoves];
        result->early_game += knightMobility[legalMoves]/2;
        if (attackingSide == white) {
            result->common     += popcount(allNorthOf(nextKnight) & getKnightMoves(nextKnight) & ~ownPieces & ~opppawnTakes)/2;
        } else {
            result->common     += popcount(allSouthOf(nextKnight) & getKnightMoves(nextKnight) & ~ownPieces & ~opppawnTakes)/2;
        }
#else*/
        result->common += knightMobility[legalMoves];

    }
    retTable.attackTables[knight] = knightAttackTable;

    //bishops
    uint64_t occupancy  = position->pieces[white] | position->pieces[black];

    uint64_t bishops = position->pieceTables[attackingSide][bishop];
    uint64_t bishopAttackTable = 0;
    while (bishops != 0) {
        uint16_t nextPieceField = popLSB(bishops);
        uint64_t potentialMoves =  getPotentialBishopMoves(nextPieceField, occupancy);
        bishopAttackTable = bishopAttackTable | potentialMoves;
        uint16_t legalMoves = popcount(potentialMoves & ~ownPieces & ~opppawnTakes);
        assert(legalMoves < 14);
/*#ifdef EXPERIMENTAL
        result->common      += bishopMobility[legalMoves];
        result->early_game  += bishopMobility[legalMoves]/2;
        if (attackingSide == white) {
            result->common     += popcount(allNorthOf(nextPieceField) & potentialMoves & ~ownPieces & ~opppawnTakes)/2;
        } else {
            result->common     += popcount(allSouthOf(nextPieceField) & potentialMoves & ~ownPieces & ~opppawnTakes)/2;
        }
#else*/
        result->common += bishopMobility[legalMoves];
//#endif
    }
    retTable.attackTables[bishop] = bishopAttackTable;
    //rooks

    uint64_t rooks = position->pieceTables[attackingSide][rook];
    uint64_t rookAttackTable = 0;
    while (rooks != 0) {
        uint16_t nextPieceField = popLSB(rooks);
        uint64_t potentialMoves =  getPotentialRookMoves(nextPieceField, occupancy);
        uint16_t legalMoves = popcount(potentialMoves & ~ownPieces);
        assert(legalMoves < 15);
/*#ifdef EXPERIMENTAL
        result->endgame += rookMobility[legalMoves]/2;
        result->common  += rookMobility[legalMoves]/2;
        if (attackingSide == white) {
            result->common     += popcount(allNorthOf(nextPieceField) & potentialMoves & ~ownPieces & ~opppawnTakes)/2;
        } else {
            result->common     += popcount(allSouthOf(nextPieceField) & potentialMoves & ~ownPieces & ~opppawnTakes)/2;
        }
#else*/
        result->common += rookMobility[legalMoves];
//#endif
        rookAttackTable = rookAttackTable | potentialMoves;
    }
    retTable.attackTables[rook] = rookAttackTable;

    //queens


    uint64_t queens = position->pieceTables[attackingSide][queen];
    uint64_t queenAttackTable = 0;
    while (queens != 0) {
        uint16_t nextPieceField = popLSB(queens);
        uint64_t potentialMoves =  getPotentialRookMoves(nextPieceField, occupancy);
        queenAttackTable = queenAttackTable | potentialMoves;
        result->common += popcount(potentialMoves & ~ownPieces);
    }

    //TODO: this could be optimized by merging with the rook table and doing both at once
    queens = position->pieceTables[attackingSide][queen];
    while (queens != 0) {
        uint16_t nextPieceField = popLSB(queens);
        uint64_t potentialMoves =  getPotentialBishopMoves(nextPieceField, occupancy);;
        queenAttackTable = queenAttackTable | potentialMoves;
        result->common += popcount(potentialMoves & ~ownPieces);
    }
    retTable.attackTables[queen] = queenAttackTable;


    //kings
    uint64_t kingField = position->pieceTables[attackingSide][king];
    uint64_t kingAttackTable = 0;
    while (kingField) {
        uint16_t nextKing = popLSB(kingField);
        kingAttackTable = kingAttackTable | getKingMoves(nextKing);
    }
    retTable.attackTables[king] = kingAttackTable;
    retTable.completeAttackTable = retTable.attackTables[king] | retTable.attackTables[pawn] | retTable.attackTables[knight] | retTable.attackTables[bishop] | retTable.attackTables[rook] | retTable.attackTables[queen];

    /*result->early_game  = 0;
    result->endgame     = 0;*/
    return retTable;
}



static inline bool getNextCapture(chessMove* nextCapture, const chessPosition* position, uint16_t field, figureType occ, uint64_t mask) {
    //TODO: pass the last figure type in so that we can immediately skip the lower types
    playerColor attackingSide = position->toMove;
    //pawns
    uint64_t potentialTakes;
    if (attackingSide == white) {
        potentialTakes = ((BIT64(field) >> 9) & NOTFILEH)| ((BIT64(field) >> 7) & NOTFILEA);
    } else {
        potentialTakes = ((BIT64(field) << 9) & NOTFILEA) | ((BIT64(field) << 7) & NOTFILEH);
    }
    uint64_t toCheck = potentialTakes & position->pieceTables[attackingSide][pawn] & ~mask;
    if (toCheck) {
        uint16_t source = findLSB(toCheck);
        //if (!(BIT64(source) & mask)) {
        nextCapture->captureType = occ;
        nextCapture->sourceField = source;
        nextCapture->targetField = field;
        nextCapture->type        = pawnMove;
        return true;
        //}
    }


    //knights
    uint64_t knights = position->pieceTables[attackingSide][knight] & ~mask;
    if (getKnightMoves(field) & knights) {
        uint16_t source = findLSB(getKnightMoves(field) & knights & ~mask);
        nextCapture->captureType = occ;
        nextCapture->sourceField = source;
        nextCapture->targetField = field;
        nextCapture->type        = knightMove;
        return true;
    }
    uint64_t occupancy = ((position->pieces[white]) | (position->pieces[black])) & ~mask;

    //bishops
    uint64_t potentialMoves = getPotentialBishopMoves(field, occupancy);

    if (potentialMoves & (position->pieceTables[attackingSide][bishop] & ~mask)) {
        uint16_t source = findLSB(potentialMoves & (position->pieceTables[attackingSide][bishop] & ~mask));
        nextCapture->captureType = occ;
        nextCapture->sourceField = source;
        nextCapture->targetField = field;
        nextCapture->type        = bishopMove;
        return true;
    }


    //rooks
    potentialMoves =  getPotentialRookMoves(field, occupancy);

    if (potentialMoves & (position->pieceTables[attackingSide][rook]  & ~mask)) {
        uint16_t source = findLSB(potentialMoves & (position->pieceTables[attackingSide][rook] & ~mask));
        nextCapture->captureType = occ;
        nextCapture->sourceField = source;
        nextCapture->targetField = field;
        nextCapture->type        = rookMove;
        return true;
    }

    //queens
    potentialMoves =  getPotentialRookMoves(field, occupancy);

    if (potentialMoves & (position->pieceTables[attackingSide][queen]  & ~mask)) {
        uint16_t source = findLSB(potentialMoves & (position->pieceTables[attackingSide][queen] & ~mask));
        nextCapture->captureType = occ;
        nextCapture->sourceField = source;
        nextCapture->targetField = field;
        nextCapture->type        = queenMove;
        return true;
    }


    potentialMoves =  getPotentialBishopMoves(field, occupancy);

    if (potentialMoves & position->pieceTables[attackingSide][queen]  & ~mask) {
        uint16_t source = findLSB(potentialMoves & (position->pieceTables[attackingSide][queen] & ~mask));
        nextCapture->captureType = occ;
        nextCapture->sourceField = source;
        nextCapture->targetField = field;
        nextCapture->type        = queenMove;
        return true;
    }
    //kings
    uint64_t kings = position->pieceTables[attackingSide][king]  & ~mask;
    if (getKingMoves(field) & kings) {
        uint16_t source = findLSB(getKingMoves(field) & kings & ~mask);
        if (!(BIT64(source) & mask)) {
            nextCapture->captureType = occ;
            nextCapture->sourceField = source;
            nextCapture->targetField = field;
            nextCapture->type        = kingMove;
            return true;
        }
    }
    return false;
}


int16_t see_internal(int16_t previous, chessPosition* position, uint16_t field, figureType lastCapturingPiece, const evalParameters* evalPars, uint64_t mask) {
    chessMove mv;
    if (!getNextCapture(&mv, position, field, lastCapturingPiece, mask)) {
        //no more captures available
        //std::cout << "no further captures found, returning " << -previous << std::endl;
        return -previous; //from POV of player currently moving
    }

    if (mv.captureType == king) { //best-case scenario for us
        return 10000;
    }

    int16_t standPat = -previous;

    position->toMove = (playerColor) (1-position->toMove);
    mask = mask | BIT64(mv.sourceField);
    int16_t seeVal = -see_internal(-previous+evalPars->figureValues[mv.captureType], position, field, (figureType) mv.type, evalPars, mask);
    position->toMove = (playerColor) (1-position->toMove);

    if (seeVal > standPat) {
        return seeVal;
    }
    return standPat;
}


int16_t SEE(chessPosition* position, chessMove* mv) {
    //TODO: SEE currently cannot handle promotions!!!!
    if (mv->type > 5) {
        return 0;
    }
    const evalParameters* evalPars = getEvalParameters();
    assert(evalPars->figureValues[none] == 0);
    uint16_t val = evalPars->figureValues[mv->captureType];
    uint64_t mask = BIT64(mv->sourceField);
    position->toMove = (playerColor) (1-position->toMove);
    int16_t ret = -see_internal(val, position, mv->targetField, (figureType) mv->type, evalPars, mask);
    position->toMove = (playerColor) (1-position->toMove);
    return ret;
}

EvalComponentResult evalMobility(const chessPosition* position, const evalParameters* par  __attribute__((unused)), EvalMemory* evalMemory) {
    EvalComponentResult whiteMobilityScore;
    evalMemory->attackTables[white] = makeAttackTableWithMobility(position, white, &whiteMobilityScore);
    EvalComponentResult blackMobilityScore;
    evalMemory->attackTables[black] = makeAttackTableWithMobility(position, black, &blackMobilityScore);
    EvalComponentResult ret;
    ret.common          = whiteMobilityScore.common     - blackMobilityScore.common;
    ret.endgame         = whiteMobilityScore.endgame    - blackMobilityScore.endgame;
    ret.early_game      = whiteMobilityScore.early_game - blackMobilityScore.early_game;
    return ret;
}
