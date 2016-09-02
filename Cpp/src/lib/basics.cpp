/*
 * basics.cpp
 *
 *  Created on: Sep 1, 2016
 *      Author: vabi
 */


#include "basics.hpp"
#include <iostream>
#include <string.h>

static char figureNames[2][6] = {{'P', 'K', 'N', 'R', 'Q', 'K'},
		{'p', 'k', 'n', 'r', 'q', 'k'},
};

std::string chessPositionToString(chessPosition position) {
	//Not performance-critical
	//---------------------------
	std::string ret = "";

	for (uint8_t ind=0; ind < 64; ind++) {
		uint64_t num = (1UL << ind);
		bool found = false;
		for	(uint8_t color=0; color < 2; color++) {
			for	(uint8_t figureCnt = 0; figureCnt < 6; figureCnt++) {

				if (position.pieceTables[color][figureCnt] & num) {
					ret.push_back(figureNames[color][figureCnt]);
					found = true;
				}

			}
		}

		if (!found) {
			ret.push_back('0');
		}
	}
	return ret;
}

chessPosition stringToChessPosition(std::string strposition) {
	//Not performance-critical
	//---------------------------
	chessPosition position;
	memset(&position, 0, sizeof(chessPosition));

	for (uint8_t ind=0; ind < 64; ind++) {
		char c = strposition.at(ind);
			switch(c){
				case 'K':
					position.pieces[white] 					|= (1UL << ind);
					position.pieceTables[white][king] 		|= (1UL << ind);
					break;
				case 'Q':
					position.pieceTables[white][queen] 		|= (1UL << ind);
					position.pieces[white] 					|= (1UL << ind);
					break;
				case 'R':
					position.pieceTables[white][rook] 		|= (1UL << ind);
					position.pieces[white] 					|= (1UL << ind);
					break;
				case 'B':
					position.pieceTables[white][bishop] 	|= (1UL << ind);
					position.pieces[white] 					|= (1UL << ind);
					break;
				case 'N':
					position.pieceTables[white][knight] 	|= (1UL << ind);
					position.pieces[white] 					|= (1UL << ind);
					break;
				case 'P':
					position.pieceTables[white][pawn] 		|= (1UL << ind);
					position.pieces[white] 					|= (1UL << ind);
					break;
				case 'k':
					position.pieceTables[black][king] 		|= (1UL << ind);
					position.pieces[black] 					|= (1UL << ind);
					break;
				case 'q':
					position.pieceTables[black][queen]  	|= (1UL << ind);
					position.pieces[black] 					|= (1UL << ind);
					break;
				case 'r':
					position.pieceTables[black][rook]  		|= (1UL << ind);
					position.pieces[black] 					|= (1UL << ind);
					break;
				case 'b':
					position.pieceTables[black][bishop]  	|= (1UL << ind);
					position.pieces[black] 					|= (1UL << ind);
					break;
				case 'n':
					position.pieceTables[black][knight]  	|= (1UL << ind);
					position.pieces[black] 					|= (1UL << ind);
					break;
				case 'p':
					position.pieceTables[black][pawn]  		|= (1UL << ind);
					position.pieces[black] 					|= (1UL << ind);
					break;
				default:
					break;
			}


	}
	return position;
}
