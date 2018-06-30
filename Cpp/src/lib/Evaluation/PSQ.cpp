/*
 * PSQ.cpp
 *
 *  Created on: Aug 10, 2017
 *      Author: vabi
 */
#include <stdint.h>
#include <lib/basics.hpp>
#include "PSQ.hpp"
#include "lib/basics.hpp"
#include "lib/bitfiddling.h"
#include "parameters/parameters.hpp"
#include "lib/Evaluation/evaluation.hpp"

PSQData psqData;

void setPSQEntry(figureType type, playerColor color, uint16_t field, int32_t value) {
    psqData.setPSQEntry(type, color, field, value);
}

void setEarlygamePSQEntry(figureType type, playerColor color, uint16_t field, int16_t value) {
    psqData.setEarlygamePSQEntry(type, color, field, value);
}

void setEndgamePSQEntry(figureType type, playerColor color, uint16_t field, int16_t value) {
    psqData.setEndgamePSQEntry(type, color, field, value);
}


EvalComponentResult PSQ(const chessPosition* position, const evalParameters* par  __attribute__ ((unused))) {
    int32_t bufferEndgame =  position->pieceTableEval >> 16;
    bufferEndgame  = bufferEndgame-(1 << 14);
    int32_t bufferMidgame =  position->pieceTableEval & 0xFFFF;
    bufferMidgame         =  bufferMidgame - (1 << 15);
    EvalComponentResult ret;
    ret.common = 0;
    ret.early_game = bufferMidgame;
    ret.endgame    = bufferEndgame;
    return ret;
}
