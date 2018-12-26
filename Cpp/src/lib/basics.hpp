/*
 * basics.hpp
 *
 *  Created on: Sep 1, 2016
 *      Author: vabi
 */

#ifndef LIB_BASICS_HPP_
#define LIB_BASICS_HPP_

#include <stdint.h>
#include <string>
#include <DataTypes/vdt_vector.hpp>
#include <assert.h>
#include <lib/basics.hpp>
#include <lib/bitfiddling.h>
#include "basicTypes.hpp"
#include "figureValueHashing.hpp"


struct chessMove {
    uint16_t sourceField;
    uint16_t targetField;
    moveType type;
    figureType captureType;
    int16_t sortEval;
    bool operator<(chessMove move) const {
            return sortEval > move.sortEval;
    }
};

struct pvLine {
    uint16_t numMoves;
    chessMove line[40];
};

struct pathDependentPositionData {
    uint8_t castlingRights;
    uint8_t enPassantFile; //the file of the CAPTURED pawn
    uint8_t fiftyMoveRuleCounter;
    uint64_t hash;
};

struct chessPosition {
    uint64_t pieces[2];
    uint64_t pieceTables[2][NUM_DIFFERENT_PIECES+1]; //the +1 is a dummy allowing simpler move execution.
    playerColor toMove;
    pathDependentPositionData data;
    int16_t figureEval;
    uint16_t totalFigureEval;
    uint32_t pieceTableEval;
    uint64_t zobristHash;
    uint64_t pawnHash;
    PresentPieces presentPieces;
    vdt_vector<chessMove> madeMoves;
    vdt_vector<pathDependentPositionData> dataStack;
};

enum hashFlag: uint16_t {FULLSEARCH = 0, FAILLOW = 1, FAILHIGH = 2};



struct HashEntry {
    uint32_t hashHighBits;
    uint16_t hashLower;
    uint16_t bestMove;
    hashFlag flag;
    uint16_t eval;
    uint8_t depth;
    uint8_t searchId;
    uint16_t index;
}__attribute__((packed));

struct HashBucket {
    HashEntry hashData[4];
}; //__attribute__((aligned(64)));


struct pawnHashEntry {
    uint32_t hashHighBits;
    uint16_t hashLower;
    int16_t  eval;
};

class HashTable {
    HashBucket* data;
    uint32_t length;

 public:
    HashTable() {
        length = 0;
        data   = NULL;
    }

    HashBucket* get(uint64_t key) {
        uint64_t index = key & (length-1);

#ifdef DEBUG
        if (index >= length) {
            //logError(std::string("OutOfBounds during hashtable access"));
            while (1) {}
        }
#endif
        return &data[index];
    }

    void clear() {
        memset(data, 0, sizeof(HashBucket)*length);
    }

    void realloc(uint32_t new_size) {
        assert(popcount(new_size) == 1);
        if (data != 0) {
            delete[] data;
        }

        data    = new HashBucket[new_size];
        length  = new_size;
    }
};

class timeoutException: public std::exception {
  virtual const char* what() const throw() {
    return "Timeout";
  }
};



uint64_t getRandUint64();
void debug_incremental_calculations(const chessPosition* position);
void logError(std::string msg);
void free_position(chessPosition* position);
inline figureType toFigureType(uint16_t num);
void zeroInitPosition(chessPosition* position);


#define INLINE __attribute__((always_inline)) static inline

#endif /* LIB_BASICS_HPP_ */
