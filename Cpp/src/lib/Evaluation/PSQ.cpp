/*
 * PSQ.cpp
 *
 *  Created on: Aug 10, 2017
 *      Author: vabi
 */
#include <stdint.h>
#include <lib/basics.hpp>
#include "PSQ.hpp"

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
