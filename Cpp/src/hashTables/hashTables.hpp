/*
 * hashTables.hpp
 *
 *  Created on: Sep 23, 2016
 *      Author: vabi
 */

#ifndef HASHTABLES_HASHTABLES_HPP_
#define HASHTABLES_HASHTABLES_HPP_


#define HASHSIZE 0xFFFFFFF

void fillZobristHash();
uint64_t calcZobristHash(const chessPosition* position);


#endif /* HASHTABLES_HASHTABLES_HPP_ */
