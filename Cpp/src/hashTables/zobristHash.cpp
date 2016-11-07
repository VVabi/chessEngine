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

hashEntry moveOrderingHashTable[HASHSIZE+1];

uint64_t zobristHash[7][2][64];
uint64_t movingSideHash[2];
uint64_t castlingHash[16];
uint64_t enpassantHash[9];

void fillZobristHash(){
	assert(popcount(HASHSIZE+1)==1); //this needs to be a power of 2!

	memset(moveOrderingHashTable, 0, sizeof(hashEntry)*(HASHSIZE+1));

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

