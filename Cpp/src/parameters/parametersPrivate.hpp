/*
 * parametersPrivate.hpp
 *
 *  Created on: Feb 2, 2017
 *      Author: vabi
 */

#include <stdint.h>



struct preParameters{
	int64_t pawnValue;
	int64_t knightValue;
	int64_t bishopValue;
	int64_t rookValue;
	int64_t queenValue;
	int64_t bishoppair;
	int64_t rookonopenfile;
	int64_t isolateddoublepawns;
	int64_t nonisolateddoublepawns;
	int64_t isolatedpawns;
	int64_t selfopenfiletoking;
	int64_t selfopenfilenexttoking;
	int64_t opponentopenfiletoking;
	int64_t opponentopenfilenexttoking;
};


