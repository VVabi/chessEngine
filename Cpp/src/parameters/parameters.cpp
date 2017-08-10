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

evalParameters evaluationParameters;
preParameters par;
extern int32_t completePieceTables[7][2][64];
extern int16_t pieceTables[7][2][64];
extern int16_t endGamepieceTables[7][2][64];
extern int16_t rawPieceTables[6][32];
extern int16_t endgameRawPieceTables[6][32];

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
}

preParameters* getPreParameters() {
    return &par;
}

staticPawnEvalParameters  initializeStaticPawnParameters(const preParameters par) {
    staticPawnEvalParameters ret_par;
    ret_par.isolatedDoublePawn      = par.isolateddoublepawns;
    ret_par.isolatedPawn            = par.isolatedpawns;
    ret_par.nonIsolatedDoublePawn   = par.nonisolateddoublepawns;
    return ret_par;
}


kingSafetyEvalParameters initializeKingSafetyParameters(const preParameters par) {
    kingSafetyEvalParameters ret;
    ret.opponentopenfilenexttoking = par.opponentopenfilenexttoking;
    ret.opponentopenfiletoking     = par.opponentopenfiletoking;
    ret.selfopenfilenexttoking     = par.selfopenfilenexttoking;
    ret.selfopenfiletoking         = par.selfopenfiletoking;
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
    evaluationParameters.staticPawnParameters       = initializeStaticPawnParameters(par);
    evaluationParameters.kingSafetyParameters       = initializeKingSafetyParameters(par);

    for (uint16_t type = 0; type < 6; type++) {
        for (uint16_t field = 0; field < 32; field++) {
            int16_t value = rawPieceTables[type][field];
            uint16_t file = field % 4;
            uint16_t row = field/4;
            pieceTables[type][white][8*row+file] = value;
            pieceTables[type][white][8*row+7-file] = value;
            pieceTables[type][black][8*(7-row)+file] = value;
            pieceTables[type][black][8*(7-row)+7-file] = value;
        }
    }

    for (uint16_t type = 0; type < 6; type++) {
        for (uint16_t field = 0; field < 32; field++) {
            int16_t value = endgameRawPieceTables[type][field];
            uint16_t file = field % 4;
            uint16_t row = field/4;
            endGamepieceTables[type][white][8*row+file] = value;
            endGamepieceTables[type][white][8*row+7-file] = value;
            endGamepieceTables[type][black][8*(7-row)+file] = value;
            endGamepieceTables[type][black][8*(7-row)+7-file] = value;
        }
    }

    for (uint16_t ind = 0; ind < 2; ind++) {
        for (uint16_t field = 0; field < 64; field++) {
            endGamepieceTables[6][ind][field] = 0;
            pieceTables[6][ind][field] = 0;
        }
    }

    const evalParameters* evalPars =  getEvalParameters();
    for (uint32_t index = 0; index < 7; index++) {
        for (uint32_t t = 0; t < 2; t++) {
            for (uint32_t k = 0; k < 64; k++) {
                completePieceTables[index][t][k] = ((uint16_t) (pieceTables[index][t][k]+evalPars->figureValues[index])) |  (((uint16_t) (endGamepieceTables[index][t][k]+evalPars->figureValues[index])) << 16);
            }
        }
    }


    //std::cout << "ENDGAME" << std::endl;
    for (uint32_t index = 0; index < 7; index++) {
            for (uint32_t t = 0; t < 2; t++) {
                for (uint32_t k = 0; k < 64; k++) {
                        int16_t val1 = endGamepieceTables[index][t][k];
                        uint16_t file = FILE(k);
                        uint16_t row = ROW(k);
                        uint16_t mirrorFile = 7-file;
                        uint16_t field = 8*row+mirrorFile;
                        int16_t val2 = endGamepieceTables[index][t][field];

                        uint16_t bfield = (7-row)*8+file;
                        int16_t val3 = endGamepieceTables[index][1-t][bfield];

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
                        int16_t val1 = pieceTables[index][t][k];
                        uint16_t file = FILE(k);
                        uint16_t row = ROW(k);
                        file = 7-file;
                        uint16_t field = 8*row+file;
                        int16_t val2 = pieceTables[index][t][field];
                        uint16_t bfield = (7-row)*8+file;
                        int16_t val3 = pieceTables[index][1-t][bfield];
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
