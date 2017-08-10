/*
 * PSQ.hpp
 *
 *  Created on: Aug 10, 2017
 *      Author: vabi
 */

#ifndef LIB_EVALUATION_PSQ_HPP_
#define LIB_EVALUATION_PSQ_HPP_
#include <stdint.h>
#include <lib/basics.hpp>
#include <assert.h>

class PSQData {
    int32_t completePieceTables[7][2][64];
    int16_t pieceTables[7][2][64];
    int16_t endGamepieceTables[7][2][64];

 public:
    int32_t getPSQentry(moveType type, playerColor color, uint16_t field) {
    #ifdef DEBUG
        assert(type < 7);
        assert(color < 2);
        assert(field < 64);
    #endif
        return completePieceTables[type][color][field];
    }
    int32_t getPSQentry(figureType type, playerColor color, uint16_t field) {
    #ifdef DEBUG
        assert(type < 7);
        assert(color < 2);
        assert(field < 64);
    #endif
        return completePieceTables[type][color][field];
    }
    int16_t getEarlyGamePSQentry(moveType type, playerColor color, uint16_t field) {
    #ifdef DEBUG
        assert(type < 7);
        assert(color < 2);
        assert(field < 64);
    #endif
        return pieceTables[type][color][field];
    }
    int16_t getEarlyGamePSQentry(figureType type, playerColor color, uint16_t field) {
    #ifdef DEBUG
        assert(type < 7);
        assert(color < 2);
        assert(field < 64);
    #endif
        return pieceTables[type][color][field];
    }
    int16_t getEndgameGamePSQentry(moveType type, playerColor color, uint16_t field) {
    #ifdef DEBUG
        assert(type < 7);
        assert(color < 2);
        assert(field < 64);
    #endif
        return endGamepieceTables[type][color][field];
    }
    int16_t getEndgameGamePSQentry(figureType type, playerColor color, uint16_t field) {
    #ifdef DEBUG
        assert(type < 7);
        assert(color < 2);
        assert(field < 64);
    #endif
        return endGamepieceTables[type][color][field];
    }

    void setPSQEntry(figureType type, playerColor color, uint16_t field, int32_t value) {
        completePieceTables[type][color][field] = value;
    }
    void setEarlygamePSQEntry(figureType type, playerColor color, uint16_t field, int16_t value) {
        pieceTables[type][color][field] = value;
    }
    void setEndgamePSQEntry(figureType type, playerColor color, uint16_t field, int16_t value) {
        endGamepieceTables[type][color][field] = value;
    }
};

extern PSQData psqData;

void setPSQEntry(figureType type, playerColor color, uint16_t field, int32_t value);
void setEarlygamePSQEntry(figureType type, playerColor color, uint16_t field, int16_t value);
void setEndgamePSQEntry(figureType type, playerColor color, uint16_t field, int16_t value);

__attribute__((always_inline)) static inline int32_t getPSQentry(moveType type, playerColor color, uint16_t field) {
    return psqData.getPSQentry(type, color, field);
}

__attribute__((always_inline)) static inline int32_t getPSQentry(figureType type, playerColor color, uint16_t field) {
    return psqData.getPSQentry(type, color, field);
}

__attribute__((always_inline)) static inline int16_t getEarlyGamePSQentry(moveType type, playerColor color, uint16_t field) {
    return psqData.getEarlyGamePSQentry(type, color, field);
}

__attribute__((always_inline)) static inline int16_t getEarlyGamePSQentry(figureType type, playerColor color, uint16_t field) {
    return psqData.getEarlyGamePSQentry(type, color, field);
}

__attribute__((always_inline)) static inline int16_t getEndgameGamePSQentry(moveType type, playerColor color, uint16_t field) {
    return psqData.getEndgameGamePSQentry(type, color, field);
}

__attribute__((always_inline)) static inline int16_t getEndgameGamePSQentry(figureType type, playerColor color, uint16_t field) {
    return psqData.getEndgameGamePSQentry(type, color, field);
}


#endif /* LIB_EVALUATION_PSQ_HPP_ */
