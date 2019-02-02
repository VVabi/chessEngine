/*
 * parameters.hpp
 *
 *  Created on: Feb 2, 2017
 *      Author: vabi
 */

#ifndef PARAMETERS_PARAMETERS_HPP_
#define PARAMETERS_PARAMETERS_HPP_
#include "parametersPrivate.hpp"

struct staticPawnEvalParameters {
    int16_t isolatedPawnTable[64];
    int16_t doublePawn; //subtracted for BOTH pawns of a doubled pawn!
    int16_t unresolvableDoublePawn;
    int16_t backwardsPawn;
};

struct kingSafetyEvalParameters {
    int16_t selfopenfiletoking;
    int16_t selfopenfilenexttoking;
    int16_t opponentopenfiletoking;
    int16_t opponentopenfilenexttoking;
    int16_t attacksCloseToKingEvals[70];
    int16_t attackScores[5];
};

struct trappedPiecesEvalParameters {
    int16_t trappedValue;
};

struct passedPawnEvalParameters {
    int16_t passedPawnEvalValues[2][64];
    int16_t kingToPromotionFieldDistance[7][7];
};

struct SpaceEvalParameters {
    int16_t figuresInOppHalf[9];
};

struct MobilityParameters {
     int16_t bishopMobility[14];
     int16_t rookMobility[15];
     int16_t knightMobility[9];
};

struct evalParameters {
    int16_t figureValues[7];
    int16_t bishoppair;
    int16_t rookOnOpenFile;
    int16_t outposts;
    int16_t hangingPieces;
    staticPawnEvalParameters staticPawnParameters;
    kingSafetyEvalParameters kingSafetyParameters;
    trappedPiecesEvalParameters  trappedPiecesParameters;
    passedPawnEvalParameters passedPawnParameters;
    SpaceEvalParameters     spaceParameters;
    MobilityParameters      mobilityParameters;
};

extern evalParameters evaluationParameters;
inline const evalParameters* getEvalParameters() {
    return &evaluationParameters;
}

void initializeParameters();
preParameters* getPreParameters();
void initializeDependentParameters(preParameters par);
int16_t getEndgameRawPieceTableEntry(uint16_t type, uint16_t field);
int16_t getRawPieceTableEntry(uint16_t type, uint16_t field);
#endif /* PARAMETERS_PARAMETERS_HPP_ */
