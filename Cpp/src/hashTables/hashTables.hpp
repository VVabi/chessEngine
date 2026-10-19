/*
 * hashTables.hpp
 *
 *  Created on: Sep 23, 2016
 *      Author: vabi
 */

#ifndef HASHTABLES_HASHTABLES_HPP_
#define HASHTABLES_HASHTABLES_HPP_

#include <lib/basics.hpp>
#include <lib/basicTypes.hpp>
#include <cassert>


#define DEFAULT_HASHSIZE 0x200000 //needs to be a power of 2 -1!

class ZobristHashData {
    uint64_t zobristHash[7][2][64];
    uint64_t pawnHashValues[7][2][64];
    uint64_t movingSideHash[2];
    uint64_t castlingHash[16];
    uint64_t enpassantHash[9];

    void setZobristHashEntry(figureType type, playerColor color, uint16_t field, uint64_t value) {
    #ifdef DEBUG
        assert(type < 7);
        assert(color < 2);
        assert(field < 64);
    #endif
        zobristHash[type][color][field] = value;
    }
    void setCastlingHash(uint16_t index, uint64_t value) {
    #ifdef DEBUG
        assert(index < 16);
    #endif
        castlingHash[index] = value;
    }

    void setEnPassantHash(uint16_t index, uint64_t value) {
    #ifdef DEBUG
        assert(index < 9);
    #endif
        enpassantHash[index] = value;
    }

    void setMovingSideHash(playerColor color, uint64_t value) {
    #ifdef DEBUG
            assert(color < 2);
    #endif
        movingSideHash[color] = value;
    }

    void setPawnZobristHashEntry(figureType type, playerColor color, uint16_t field, uint64_t value) {
    #ifdef DEBUG
        assert(type < 7);
        assert(color < 2);
        assert(field < 64);
    #endif
        pawnHashValues[type][color][field] = value;
    }

 public:
    void init();

    uint64_t getCastlingHash(uint16_t index) const {
    #ifdef DEBUG
        assert(index < 16);
    #endif
        return castlingHash[index];
    }

    uint64_t getEnPassantHash(uint16_t index) const {
    #ifdef DEBUG
        assert(index < 9);
    #endif
        return enpassantHash[index];
    }

    uint64_t getMovingSideHash(playerColor color) const {
    #ifdef DEBUG
            assert(color < 2);
    #endif
        return movingSideHash[color];
    }

    uint64_t getZobristHashEntry(figureType type, playerColor color, uint16_t field) const {
    #ifdef DEBUG
        assert(type < 7);
        assert(color < 2);
        assert(field < 64);
    #endif
        return zobristHash[type][color][field];
    }

    uint64_t getPawnHashEntry(figureType type, playerColor color, uint16_t field) const {
    #ifdef DEBUG
        assert(type < 7);
        assert(color < 2);
        assert(field < 64);
    #endif
        return pawnHashValues[type][color][field];
    }
};

extern ZobristHashData hashData;

__attribute__((always_inline)) static inline uint64_t getHashEntry(figureType type, playerColor color, uint16_t field) {
    return hashData.getZobristHashEntry(type, color, field);
}

__attribute__((always_inline)) static inline uint64_t getPawnZobristHashEntry(figureType type, playerColor color, uint16_t field) {
    return hashData.getPawnHashEntry(type, color, field);
}

__attribute__((always_inline)) static inline uint64_t getMovingSideHash(playerColor color) {
    return hashData.getMovingSideHash(color);
}

__attribute__((always_inline)) static inline uint64_t getCastlingHash(uint16_t index) {
    return hashData.getCastlingHash(index);
}

__attribute__((always_inline)) static inline uint64_t getEnPassantHash(uint16_t index) {
    return hashData.getEnPassantHash(index);
}

void initZobristHashValues();
void initHashTables();
void reallocHashTables(uint32_t new_size);
uint64_t calcZobristHash(const chessPosition* position);
PresentPieces calcPresentPieces(const chessPosition* position);
uint64_t calcPawnHash(const chessPosition* position);
HashEntry getHashTableEntry(uint64_t zobristKey);
void setPawnHashEntry(int16_t eval,  uint64_t key);
bool getPawnHashTableEntry(pawnHashEntry* entry, uint64_t key);
void setHashEntry(HashEntry entry, uint64_t zobristKey);
uint16_t getHashMove(uint64_t zobristKey);
void setHashMove(uint16_t move, uint64_t zobristKey, uint8_t searchId);
void setHashEntry(hashFlag flag, int16_t eval, uint8_t depth, uint8_t searchId, uint16_t bestMove, uint64_t key);

//Mate scores are relative to the root during the search, but stored relative to the node in the hash table,
//since the same position can be reached at different plies. Apply these to every store and every probe
//-----------------------------------------------------------------------------------------------------------
inline bool isMateScore(int32_t eval) {
    return (eval > 10000 || eval < -10000) && (eval < 30001) && (eval > -30001);
}

inline int16_t evalToHash(int32_t eval, uint16_t ply) {
    if (isMateScore(eval)) {
        return (eval < 0 ? eval-ply : eval+ply);
    }
    return eval;
}

inline int16_t evalFromHash(int32_t eval, uint16_t ply) {
    if (isMateScore(eval)) {
        return (eval < 0 ? eval+ply : eval-ply);
    }
    return eval;
}
void setSearchId(uint8_t searchId, uint64_t key, uint16_t index);
void clearHashTables();
#endif /* HASHTABLES_HASHTABLES_HPP_ */
