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

static hashBucket moveOrderingHashTable[HASHSIZE+1];

uint64_t zobristHash[7][2][64];
uint64_t movingSideHash[2];
uint64_t castlingHash[16];
uint64_t enpassantHash[9];

#ifdef HASH
static uint16_t permutationIndex = 0;


static uint16_t permutations[4][4] = {{0,1,2,3}, {1,2,3,0}, {2,3,0,1}, {3,0,1,2}};




//TODO: somehow force-inline these functions everywhere

hashEntry getHashTableEntry(uint64_t zobristKey) {
	hashBucket current = moveOrderingHashTable[zobristKey & HASHSIZE];
	hashEntry ret;
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

	for(uint8_t ind=0; ind < 4; ind++){
		hashEntry entry = current.hashData[ind];
		if((entry.hashHighBits == zobristHigher) && (entry.hashLower == zobristLower)){
			ret = entry;
		}
	}
	return ret;
}

void setSearchId(uint8_t searchId, uint64_t key, uint16_t index) {
	moveOrderingHashTable[key & HASHSIZE].hashData[index].searchId = searchId;
}

void setHashEntry(hashFlag flag, int16_t eval, uint8_t depth, uint8_t searchId, uint16_t bestMove, uint64_t key){
	hashBucket* current = &moveOrderingHashTable[key & HASHSIZE];

	for(uint8_t ind=0; ind < 4; ind++){
		hashEntry* entry = &current->hashData[permutations[permutationIndex][ind]];
		if((entry->depth <= depth) || (entry->searchId != searchId)) {
			entry->flag = flag;
			entry->eval  = eval;
			entry->depth = depth;
			entry->searchId = searchId;
			entry->bestMove = bestMove;
			entry->hashHighBits = (uint32_t) (key >> 32);
			entry->hashLower    = (uint16_t) (((uint32_t) (key & 0xFFFFFFFF)) >> 16);
			entry->index        = permutations[permutationIndex][ind];
			break;
		}
	}
	permutationIndex = (permutationIndex+1) & 3;
}

uint16_t getHashMove(uint64_t zobristKey){
	hashBucket* current = &moveOrderingHashTable[zobristKey & HASHSIZE];
	uint32_t zobristHigher = (uint32_t) (zobristKey  >> 32);
	uint16_t zobristLower  = (uint16_t) (((uint32_t) (zobristKey  & 0xFFFFFFFF)) >> 16);
	for(uint8_t ind=0; ind < 4; ind++){
		hashEntry* entry = &current->hashData[ind];
		if ((entry->hashHighBits == zobristHigher) && (entry->hashLower == zobristLower)) {
			return entry->bestMove;
		}
	}
	return 0;

}


void setHashMove(uint16_t move, uint64_t zobristKey, uint8_t searchId) {  //DEPRECATED
	hashBucket* current = &moveOrderingHashTable[zobristKey & HASHSIZE];
	uint32_t zobristHigher = (uint32_t) (zobristKey  >> 32);
	uint16_t zobristLower  = (uint16_t) (((uint32_t) (zobristKey  & 0xFFFFFFFF)) >> 16);
	for(uint8_t ind=0; ind < 4; ind++){
		hashEntry* entry = &current->hashData[permutations[permutationIndex][ind]];
		if ((entry->searchId != searchId)){
			entry->searchId = searchId;
			entry->depth    = 0;
			entry->bestMove = move;
			//to ensure we NEVER use this entry in the search
			entry->eval     = -20000;
			entry->flag     = FAILHIGH;
			entry->hashHighBits = zobristHigher;
			entry->hashLower    = zobristLower;
			entry->index        = ind;
		}
	}
	permutationIndex = (permutationIndex+1) & 3;
}
#endif

void clearHashTables() {
	memset(moveOrderingHashTable, 0, sizeof(hashBucket)*(HASHSIZE+1));
}

void fillZobristHash(){
	assert(popcount(HASHSIZE+1)==1); //this needs to be a power of 2!
	clearHashTables();
	movingSideHash[0] = getRandUint64();
	movingSideHash[1] = 0;
	for(uint16_t cnt=0; cnt < 2; cnt++){
		for(uint16_t ind=0; ind < 6; ind++){
			for(uint16_t field=0; field < 64; field++){
				zobristHash[ind][cnt][field] = getRandUint64();
			}
		}
		for(uint16_t field=0; field < 64; field++){
			zobristHash[6][cnt][field] = 0;
		}
	}

	for(uint16_t ind=0; ind <9; ind++) {
		enpassantHash[ind]  =getRandUint64();
	}

	for(uint16_t ind=0; ind <16; ind++) {
		castlingHash[ind]  =getRandUint64();
	}
}

uint64_t calcZobristHash(const chessPosition* position){
	uint64_t hash = 0;
	for(uint16_t movingSide = 0; movingSide < 2; movingSide++){
		for(uint16_t figureType = 0; figureType < 6; figureType++){
			uint64_t pieces = position->pieceTables[movingSide][figureType];
			while(pieces){
				uint16_t field = popLSB(pieces);
				hash = hash^zobristHash[figureType][movingSide][field];
			}
		}
	}

	hash = hash^enpassantHash[position->data.enPassantFile];
	hash = hash^castlingHash[position->data.castlingRights];
	hash = hash^movingSideHash[position->toMove];
	return hash;
}

