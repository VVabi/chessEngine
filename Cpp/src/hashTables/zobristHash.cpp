/*
 * zobristHash.cpp
 *
 *  Created on: Sep 23, 2016
 *      Author: vabi
 */

#include <lib/basics.hpp>
#include <stdint.h>
#include <lib/bitfiddling.h>
#include "hashTables.hpp"
#include <assert.h>
#include <iostream>

static HashTable hashTable;

static pawnHashEntry pawnHashTable[8192];

ZobristHashData hashData;

//uint64_t zobristHash[7][2][64];
//uint64_t pawnHashValues[7][2][64];

void initZobristHashValues() {
    hashData.init();
}


#ifdef HASH
//static uint16_t permutationIndex = 0;


//static uint16_t permutations[4][4] = { {0, 1, 2, 3}, {1, 2, 3, 0}, {2, 3, 0, 1}, {3, 0, 1, 2}};


bool getPawnHashTableEntry(pawnHashEntry* entry, uint64_t key) {
    uint32_t index = (key & 8191);
    uint32_t zobristHigher = (uint32_t) (key  >> 32);
    uint16_t zobristLower  = (uint16_t) (((uint32_t) (key  & 0xFFFFFFFF)) >> 16);

    pawnHashEntry tentry = pawnHashTable[index];

    if ((tentry.hashHighBits == zobristHigher) && (tentry.hashLower == zobristLower)) {
        *entry = tentry;
        return true;
    }

    return false;
}

void setPawnHashEntry(int16_t eval, uint64_t key) {
    uint32_t index = (key & 8191);
    uint32_t zobristHigher = (uint32_t) (key  >> 32);
    uint16_t zobristLower  = (uint16_t) (((uint32_t) (key  & 0xFFFFFFFF)) >> 16);

    pawnHashEntry entry;
    entry.eval = eval;
    entry.hashHighBits = zobristHigher;
    entry.hashLower = zobristLower;
    pawnHashTable[index] = entry;
}


//TODO: somehow force-inline these functions everywhere

HashEntry getHashTableEntry(uint64_t zobristKey) {
    HashBucket* current = hashTable.get(zobristKey);
    HashEntry ret;
    ret.bestMove = 0;
    ret.depth   = 0;
    ret.eval    = 0;
    ret.hashHighBits = 0;
    ret.hashLower = 0;
    ret.index = 0;
    ret.searchId = 0;
    ret.flag = FULLSEARCH;

    uint32_t zobristHigher = (uint32_t) (zobristKey  >> 32);
    uint16_t zobristLower  = (uint16_t) (((uint32_t) (zobristKey  & 0xFFFFFFFF)) >> 16);
    int16_t max_depth = -1;

    for (uint8_t ind = 0; ind < 4; ind++) {
        HashEntry entry = current->hashData[ind];
        if ((entry.hashHighBits == zobristHigher) && (entry.hashLower == zobristLower) && (entry.depth > max_depth)) {
            ret = entry;
            max_depth = entry.depth;
        }
    }
    return ret;
}

void setSearchId(uint8_t searchId, uint64_t key, uint16_t index) {
    hashTable.get(key)->hashData[index].searchId = searchId;
}

void setHashEntry(hashFlag flag, int16_t eval, uint8_t depth, uint8_t searchId, uint16_t bestMove, uint64_t key) {
    HashBucket* current = hashTable.get(key);
    //TODO: Better replacement scheme. Go through all four buckets and replace the least-likely useful one?
    int8_t replace_index = -1;
    int32_t target_score = ((int32_t) depth);

    uint32_t high = (uint32_t) (key >> 32);
    uint16_t low  =  (uint16_t) (((uint32_t) (key & 0xFFFFFFFF)) >> 16);

    for (uint8_t ind = 0; ind < 4; ind++) {
        HashEntry* entry = &current->hashData[ind];

        if (entry->hashHighBits == 0) {
            replace_index = ind;
            break;
        }

        uint8_t past_search = searchId-entry->index;
        int32_t score = ((int32_t) entry->depth)-2*((int32_t) past_search);
        //int32_t score = -((int32_t) past_search);
        if (score < target_score) {
             target_score = score;
             replace_index = ind;
        }
    }

    if (replace_index >= 0) {
        HashEntry* entry = &current->hashData[replace_index];
        entry->flag = flag;
        entry->eval  = eval;
        entry->depth = depth;
        entry->searchId = searchId;
        entry->bestMove = bestMove;
        entry->hashHighBits = high;
        entry->hashLower    = low;
        entry->index        = replace_index;
    }


    /*for (uint8_t ind = 0; ind < 4; ind++) {
        hashEntry* entry = &current->hashData[ind];
        if ((entry->depth <= depth) || (entry->searchId != searchId)) {
            entry->flag = flag;
            entry->eval  = eval;
            entry->depth = depth;
            entry->searchId = searchId;
            entry->bestMove = bestMove;
            entry->hashHighBits = (uint32_t) (key >> 32);
            entry->hashLower    = (uint16_t) (((uint32_t) (key & 0xFFFFFFFF)) >> 16);
            entry->index        = ind;
            break;
        }
    }*/
}

uint16_t getHashMove(uint64_t zobristKey) {
    HashBucket* current = hashTable.get(zobristKey);
    uint32_t zobristHigher = (uint32_t) (zobristKey  >> 32);
    uint16_t zobristLower  = (uint16_t) (((uint32_t) (zobristKey  & 0xFFFFFFFF)) >> 16);
    for (uint8_t ind = 0; ind < 4; ind++) {
        HashEntry* entry = &current->hashData[ind];
        if ((entry->hashHighBits == zobristHigher) && (entry->hashLower == zobristLower)) {
            return entry->bestMove;
        }
    }
    return 0;
}


void setHashMove(uint16_t move, uint64_t zobristKey, uint8_t searchId) {  //DEPRECATED
    HashBucket* current = hashTable.get(zobristKey);
    uint32_t zobristHigher = (uint32_t) (zobristKey  >> 32);
    uint16_t zobristLower  = (uint16_t) (((uint32_t) (zobristKey  & 0xFFFFFFFF)) >> 16);
    for (uint8_t ind = 0; ind < 4; ind++) {
        HashEntry* entry = &current->hashData[ind]; //&current->hashData[permutations[permutationIndex][ind]];
        if ((entry->searchId != searchId)) {
            entry->searchId = searchId;
            entry->depth    = 0;
            entry->bestMove = move;
            //to ensure we NEVER use this entry in the search
            entry->eval     = -20000;
            entry->flag     = FAILHIGH;
            entry->hashHighBits = zobristHigher;
            entry->hashLower    = zobristLower;
            entry->index        = ind;
            break;
        }
    }
    //permutationIndex = (permutationIndex+1) & 3;
}
#endif

void clearHashTables() {
    hashTable.clear();
}

void ZobristHashData::init() {
    setMovingSideHash(white, getRandUint64());
        setMovingSideHash(black, 0);
        for (uint16_t cnt = 0; cnt < 2; cnt++) {
            for (uint16_t ind = 0; ind < 6; ind++) {
                for (uint16_t field = 0; field < 64; field++) {
                    setZobristHashEntry((figureType) ind, (playerColor) cnt, field, getRandUint64());
                }
            }
            for (uint16_t field = 0; field < 64; field++) {
                setZobristHashEntry(none, (playerColor) cnt, field, 0);
            }
        }

        for (uint16_t ind = 0; ind <9; ind++) {
            setEnPassantHash(ind, getRandUint64());
        }

        for (uint16_t ind = 0; ind <16; ind++) {
            setCastlingHash(ind, getRandUint64());
        }


        for (uint16_t cnt = 0; cnt < 2; cnt++) {
                for (uint16_t ind = 0; ind < 1; ind++) {
                    for (uint16_t field = 0; field < 64; field++) {
                        setPawnZobristHashEntry((figureType) ind, (playerColor) cnt, field, getRandUint64());
                    }
                }
                for (uint16_t ind = 1; ind < 7; ind++) {
                    for (uint16_t field = 0; field < 64; field++) {
                        setPawnZobristHashEntry((figureType) ind, (playerColor) cnt, field, 0);
                    }
                }
        }
}

void reallocHashTables(uint32_t new_size) {
    assert(popcount(new_size) == 1); //this needs to be a power of 2!
    hashTable.realloc(new_size);
    clearHashTables();
}

void initHashTables() {
    reallocHashTables(DEFAULT_HASHSIZE);
}


uint64_t calcPawnHash(const chessPosition* position) {
    uint64_t hash = 0;
    for (uint16_t movingSide = 0; movingSide < 2; movingSide++) {
        for (uint16_t figuretype = 0; figuretype < 6; figuretype++) {
            uint64_t pieces = position->pieceTables[movingSide][figuretype];
            while (pieces) {
                uint16_t field = popLSB(pieces);
                hash = hash^getPawnZobristHashEntry((figureType) figuretype, (playerColor) movingSide, field);
            }
        }
    }
    return hash;
}

uint64_t calcZobristHash(const chessPosition* position) {
    uint64_t hash = 0;
    for (uint16_t movingSide = 0; movingSide < 2; movingSide++) {
        for (uint16_t figuretype = 0; figuretype < 6; figuretype++) {
            uint64_t pieces = position->pieceTables[movingSide][figuretype];
            while (pieces) {
                uint16_t field = popLSB(pieces);
                hash = hash^getHashEntry((figureType) figuretype, (playerColor) movingSide, field);
            }
        }
    }

    hash = hash^getEnPassantHash(position->data.enPassantFile);
    hash = hash^getCastlingHash(position->data.castlingRights);
    hash = hash^getMovingSideHash(position->toMove);
    return hash;
}

PresentPieces calcPresentPieces(const chessPosition* position) {
    PresentPieces p;
    p.reset();

    for (uint16_t movingSide = 0; movingSide < 2; movingSide ++) {
          for (uint16_t figuretype = 0; figuretype < 6; figuretype++) {
                uint64_t pieces = position->pieceTables[movingSide][figuretype];
                while (pieces) {
                    popLSB(pieces);
                    p.addPiece((playerColor) movingSide, (figureType) figuretype);
                }
            }
    }
    return p;
}
