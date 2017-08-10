/*
 * moveGenerationInternals.hpp
 *
 *  Created on: Jun 17, 2017
 *      Author: vabi
 */

#ifndef LIB_MOVEGENERATION_MOVEGENERATIONINTERNALS_HPP_
#define LIB_MOVEGENERATION_MOVEGENERATIONINTERNALS_HPP_
extern uint64_t knightmovetables[];
extern uint64_t kingmovetables[];
extern uint64_t rookFieldTable[];
extern uint64_t rookMoveTables[64][4096];
extern uint64_t rookMagicNumbers[];

extern uint64_t bishopFieldTable[];
extern uint64_t bishopMoveTables[64][512];
extern uint64_t bishopMagicNumbers[];

__attribute__((always_inline)) static inline uint64_t getPotentialRookMoves(uint16_t rookField, uint64_t occupancy) {
    uint64_t magicNumber = rookMagicNumbers[rookField];
    uint64_t blocker = occupancy & rookFieldTable[rookField];
    uint16_t hashValue = (blocker*magicNumber) >> 52;
    uint64_t potentialMoves = rookMoveTables[rookField][hashValue];
    return potentialMoves;
}

__attribute__((always_inline)) static inline uint64_t getPotentialBishopMoves(uint16_t bishopField, uint64_t occupancy) {
    uint64_t magicNumber = bishopMagicNumbers[bishopField];
    uint64_t blocker = occupancy & bishopFieldTable[bishopField];
    uint16_t hashValue = (blocker*magicNumber) >> 55;
    uint64_t potentialMoves = bishopMoveTables[bishopField][hashValue];
    return potentialMoves;
}

#endif /* LIB_MOVEGENERATION_MOVEGENERATIONINTERNALS_HPP_ */
