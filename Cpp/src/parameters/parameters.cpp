/*
 * parameters.cpp
 *
 *  Created on: Feb 2, 2017
 *      Author: vabi
 */



#include "externalParamReader.hpp"
#include <iostream>
#include "parameters.hpp"
#include <lib/basics.hpp>
#include <lib/Defines/figureValues.hpp>
#include <lib/Defines/evalParameters.hpp>
#include <lib/Evaluation/PSQ.hpp>

#define SAFE_CPY(dst, src) assert(sizeof(dst) == sizeof(src)); memcpy(dst, src, sizeof(src));

evalParameters evaluationParameters;
preParameters par;

static int16_t attacksCloseToKingEvals[] =
{ 0, 0, 1, 2, 3, 5, 8, 10, 13, 20,
 23, 26, 29, 33, 37, 41, 45, 51, 57, 63,
 69, 75, 82, 89, 96, 103, 110, 118, 127, 136,
 145, 154, 163, 172, 181, 190, 199, 208, 217, 216,
 226, 236, 246, 256, 266, 276, 286, 296, 306, 316,
 326, 336, 346, 356, 366, 376, 386, 395, 404, 413,
 421, 429, 437, 441, 450, 450, 450, 450, 450, 450
};

static int16_t attackScores[] = {1, 3, 3, 4, 7};

static int16_t const passedPawnEvalValues[2][64] = {
        { 0, 0, 0, 0, 0, 0, 0, 0,
          8, 7, 6, 5, 5, 6, 7, 8,
          16, 15, 14, 13, 13, 14, 15, 16,
          31, 29, 27, 25, 25, 27, 29, 31,
          47, 44, 42, 40, 40, 42, 44, 47,
          90, 89, 84, 80, 80, 84, 89, 90,
          135, 131, 128, 125, 125, 128, 131, 135,
          0 , 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0,
         135, 131, 128, 125, 125, 128, 131, 135,
         90, 89, 84, 80, 80, 84, 89, 90,
          47, 44, 42, 40, 40, 42, 44, 47,
          31, 29, 27, 25, 25, 27, 29, 31,
          16, 15, 14, 13, 13, 14, 15, 16,
          8, 7, 6, 5, 5, 6, 7, 8,
          0, 0, 0, 0, 0, 0, 0, 0 }
};


static int16_t kingToPromotionFieldDistance[7][7] = {  // [dist to promotion][king distance from promotion field
        {0, 0, 0, 0, 0, 0, 0 }, //never happens
        {0, 0, -10, -20, -30, -45, -60},
        {0, 0, -5, -10, -15, -25, -40 },
        {0, 0, 0, -5, -10, -15, -25 },
        {0, 0, 0, 0, -5, -10, -15 },
        {0, 0, 0, 0, -5, -5, -10 },
        {0, 0, 0, 0, -5, -5, -10 },
};

static int16_t isolatedPawnTable[] =
        {  0, 0, 0, 0, 0, 0, 0, 0,
          -5, -6, -7, -8, -8, -7, -6, -5,
          -5, -6, -8, -9, -9, -8, -6, -5,
          -7, -9, -11, -13, -13, -11, -9,
          -7, -7, -9, -11, -13, -13, -11, -9,
          -7, -5, -6, -8, -9, -9, -8, -6, -5,
          -5, -6, -7, -8, -8, -7, -6, -5,
           0, 0, 0, 0, 0, 0, 0, 0, };

static int16_t bishopMobility[14]   = {-35, -25, -18, -13, -5, 0, 3, 5, 8, 10, 12, 14, 16, 18};
static int16_t rookMobility[15]     = {-35, -25, -18, -15, -10, -8, -3, 0, 3, 5, 8, 10, 12, 14, 16};
static int16_t knightMobility[9]   = {-25, -18, -8, 0, 3, 5, 8, 10, 12};
static int16_t spaceEvals[] = {0, 5, 10, 18, 30, 45, 60, 75, 90};
void paramDefaultInit(preParameters* par) {
    par->pawnValue      = PAWNVALUE;
    par->knightValue    = KNIGHTVALUE;
    par->bishopValue    = BISHOPVALUE;
    par->rookValue      = ROOKVALUE;
    par->queenValue     = QUEENVALUE;
    par->bishoppair     = BISHOPPAIR;
    par->rookonopenfile = ROOKONOPENFILE;
    par->isolatedpawns  = ISOLATEDPAWN;
    par->isolateddoublepawns = ISOLATEDDOUBLEPAWN;
    par->nonisolateddoublepawns = NONISOLATEDDOUBLEPAWN;
    par->opponentopenfilenexttoking = OPPONENTOPENFILENEXTTOKING;
    par->opponentopenfiletoking     = OPPONENTOPENFILETOKING;
    par->selfopenfilenexttoking     = SELFOPENFILENEXTTOKING;
    par->selfopenfiletoking         = SELFOPENFILETOKING;
    par->trappedPieces              = 50;
    par->outposts                   = 15;
}

preParameters* getPreParameters() {
    return &par;
}

staticPawnEvalParameters initializeStaticPawnParameters() {
    staticPawnEvalParameters ret_par;
    SAFE_CPY(ret_par.isolatedPawnTable, isolatedPawnTable);
    ret_par.doublePawn                  = -10;
    ret_par.unresolvableDoublePawn      = -10;
    ret_par.backwardsPawn               = -3;
    return ret_par;
}


kingSafetyEvalParameters initializeKingSafetyParameters(const preParameters par) {
    kingSafetyEvalParameters ret;
    ret.opponentopenfilenexttoking = par.opponentopenfilenexttoking;
    ret.opponentopenfiletoking     = par.opponentopenfiletoking;
    ret.selfopenfilenexttoking     = par.selfopenfilenexttoking;
    ret.selfopenfiletoking         = par.selfopenfiletoking;
    SAFE_CPY(ret.attacksCloseToKingEvals, attacksCloseToKingEvals);
    SAFE_CPY(ret.attackScores, attackScores);
    return ret;
}


MobilityParameters initializeMobilityParameters() {
    MobilityParameters ret;
    SAFE_CPY(ret.bishopMobility, bishopMobility);
    SAFE_CPY(ret.knightMobility, knightMobility);
    SAFE_CPY(ret.rookMobility,   rookMobility);
    return ret;
}

void initializeDependentParameters(preParameters par) {
    evaluationParameters.figureValues[pawn]         = par.pawnValue;
    evaluationParameters.figureValues[knight]       = par.knightValue;
    evaluationParameters.figureValues[bishop]       = par.bishopValue;
    evaluationParameters.figureValues[rook]         = par.rookValue;
    evaluationParameters.figureValues[queen]        = par.queenValue;
    evaluationParameters.figureValues[king]         = 10000;
    evaluationParameters.figureValues[none]         = 0;
    evaluationParameters.bishoppair                 = par.bishoppair;
    evaluationParameters.rookOnOpenFile             = par.rookonopenfile;
    evaluationParameters.staticPawnParameters       = initializeStaticPawnParameters();
    evaluationParameters.kingSafetyParameters       = initializeKingSafetyParameters(par);
    evaluationParameters.trappedPiecesParameters.trappedValue = par.trappedPieces;
    evaluationParameters.outposts                   = par.outposts;
    evaluationParameters.mobilityParameters         = initializeMobilityParameters();
    evaluationParameters.hangingPieces              = -100;

    SAFE_CPY(evaluationParameters.spaceParameters.figuresInOppHalf, spaceEvals);
    SAFE_CPY(evaluationParameters.passedPawnParameters.passedPawnEvalValues, passedPawnEvalValues);
    SAFE_CPY(evaluationParameters.passedPawnParameters.kingToPromotionFieldDistance, kingToPromotionFieldDistance);

    for (uint16_t type = 0; type < 6; type++) {
        for (uint16_t field = 0; field < 32; field++) {
            int16_t value = getRawPieceTableEntry(type, field);
            uint16_t file = field % 4;
            uint16_t row = field/4;
            setEarlygamePSQEntry((figureType) type, white, 8*row+file, value);
            setEarlygamePSQEntry((figureType) type, white, 8*row+7-file, value);
            setEarlygamePSQEntry((figureType) type, black, 8*(7-row)+file, value);
            setEarlygamePSQEntry((figureType) type, black, 8*(7-row)+7-file, value);
        }
    }

    for (uint16_t type = 0; type < 6; type++) {
        for (uint16_t field = 0; field < 32; field++) {
            int16_t value = getEndgameRawPieceTableEntry(type, field);
            uint16_t file = field % 4;
            uint16_t row = field/4;
            setEndgamePSQEntry((figureType) type, white, 8*row+file, value);
            setEndgamePSQEntry((figureType) type, white, 8*row+7-file, value);
            setEndgamePSQEntry((figureType) type, black, 8*(7-row)+file, value);
            setEndgamePSQEntry((figureType) type, black, 8*(7-row)+7-file, value);
        }
    }

    for (uint16_t ind = 0; ind < 2; ind++) {
        for (uint16_t field = 0; field < 64; field++) {
            setEndgamePSQEntry(none, (playerColor) ind, field, 0);
            setEarlygamePSQEntry(none, (playerColor) ind, field, 0);
        }
    }

    const evalParameters* evalPars =  getEvalParameters();
    for (uint32_t index = 0; index < 7; index++) {
        for (uint32_t t = 0; t < 2; t++) {
            for (uint32_t k = 0; k < 64; k++) {
                int32_t toSet = ((uint16_t) (getEarlyGamePSQentry((figureType) index, (playerColor) t, k)+evalPars->figureValues[index])) |  (((uint16_t) (getEndgameGamePSQentry((figureType) index, (playerColor) t, k)+evalPars->figureValues[index])) << 16);
                setPSQEntry((figureType) index, (playerColor) t, k, toSet);
            }
        }
    }


    //std::cout << "ENDGAME" << std::endl;
    for (uint32_t index = 0; index < 7; index++) {
            for (uint32_t t = 0; t < 2; t++) {
                for (uint32_t k = 0; k < 64; k++) {
                        int16_t val1 = getEndgameGamePSQentry((figureType) index, (playerColor) t, k);
                        uint16_t file = FILE(k);
                        uint16_t row = ROW(k);
                        uint16_t mirrorFile = 7-file;
                        uint16_t field = 8*row+mirrorFile;
                        int16_t val2 = getEndgameGamePSQentry((figureType) index, (playerColor) t, field);

                        uint16_t bfield = (7-row)*8+file;
                        int16_t val3 = getEndgameGamePSQentry((figureType) index, INVERTCOLOR(t), bfield);

                        if (val1 != val2) {
                            std::cout << "East/west" << std::endl;
                            std::cout << index << " " << t << " " << k << std::endl;
                        }

                        if (val1 != val3) {
                            std::cout << "North/South" << std::endl;
                            std::cout << index << " " << t << " " << k << std::endl;
                        }
                }
            }
    }
    //std::cout << "MIDGAME" << std::endl;
    for (uint32_t index = 0; index < 7; index++) {
            for (uint32_t t = 0; t < 2; t++) {
                for (uint32_t k = 0; k < 64; k++) {
                        int16_t val1 = getEarlyGamePSQentry((figureType) index, (playerColor) t, k);
                        uint16_t file = FILE(k);
                        uint16_t row = ROW(k);
                        file = 7-file;
                        uint16_t field = 8*row+file;
                        int16_t val2 = getEarlyGamePSQentry((figureType) index, (playerColor) t, field);
                        uint16_t bfield = (7-row)*8+file;
                        int16_t val3 = getEarlyGamePSQentry((figureType) index, INVERTCOLOR(t), bfield);
                        if (val1 != val2) {
                            std::cout << "East/west" << std::endl;
                            std::cout << index << " " << t << " " << k << std::endl;
                        }

                        if (val1 != val3) {
                            std::cout << "North/South" << std::endl;
                            std::cout << index << " " << t << " " << k << std::endl;
                        }
                }
            }
    }
}

void initializeParameters() {
    parameterReader reader = parameterReader();
    paramDefaultInit(&par);
    reader.readParameters(&par, "params.txt");
    /*std::cout << par.pawnValue << std::endl;
    std::cout << par.knightValue << std::endl;
    std::cout << par.bishopValue << std::endl;
    std::cout << par.rookValue << std::endl;
    std::cout << par.queenValue << std::endl;*/
    initializeDependentParameters(par);
}
