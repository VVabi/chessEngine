/*
 * hashTables.hpp
 *
 *  Created on: Sep 23, 2016
 *      Author: vabi
 */

#ifndef HASHTABLES_HASHTABLES_HPP_
#define HASHTABLES_HASHTABLES_HPP_


#define HASHSIZE 0x1FFFFF //needs to be a power of 2!

void fillZobristHash();
uint64_t calcZobristHash(const chessPosition* position);
uint64_t calcPawnHash(const chessPosition* position);
hashEntry getHashTableEntry(uint64_t zobristKey);
void setPawnHashEntry(int16_t eval, uint8_t whiteColumns, uint8_t blackColumns, uint64_t key);
bool getPawnHashTableEntry(pawnHashEntry* entry, uint64_t key);
void setHashEntry(hashEntry entry, uint64_t zobristKey);
uint16_t getHashMove(uint64_t zobristKey);
void setHashMove(uint16_t move, uint64_t zobristKey, uint8_t searchId);
void setHashEntry(hashFlag flag, int16_t eval, uint8_t depth, uint8_t searchId, uint16_t bestMove, uint64_t key);
void setSearchId(uint8_t searchId, uint64_t key, uint16_t index);

#endif /* HASHTABLES_HASHTABLES_HPP_ */
