/*
 * helperFunctions.cpp
 *
 *  Created on: Sep 3, 2016
 *      Author: vabi
 */

#include <stdio.h>
#include <string>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <stdint.h>
#include <iostream>
#include <lib/basics.hpp>

std::string getZeroPosition() {
	return std::string(64, '0');
}

static char randomTokens[] = {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'p', 'n', 'b', 'r', 'q', 'k',  'P', 'N', 'B', 'R', 'Q', 'K'  };

static char figures[2][6] = { {'P', 'N', 'B', 'R', 'Q', 'K'}, {'p', 'n', 'b', 'r', 'q', 'k'}};

std::string getRandomPosition(playerColor color) {
	srand (time(NULL));

	std::string ret = "";

	for(uint8_t ind=0; ind < 64; ind++) {
		uint32_t index  = rand() % 24;
		ret.push_back(randomTokens[index]);
	}

	if (color == black) {
		ret.push_back('b');
	} else {
		ret.push_back('w');
	}

	ret.push_back('0');
	ret.push_back('0');
	ret.push_back('0');
	ret.push_back('0');
	return ret;
}


char getRandomPiece(playerColor color) {

	srand (time(NULL));
	uint32_t index  = rand() % 6;

	return figures[color][index];


}


uint32_t getRandomField(){
	srand (time(NULL));
	uint32_t index  = rand() % 64;
	return index;
}

figureType getFigureTypeFromChar(char piece) {

	figureType type = pawn;

	switch(piece) {

		case 'P':
		case 'p':
			type = pawn;
			break;
		case 'N':
		case 'n':
			type = knight;
			break;
		case 'B':
		case 'b':
			type = bishop;
			break;
		case 'R':
		case 'r':
			type = rook;
			break;
		case 'Q':
		case 'q':
			type = queen;
			break;
		case 'K':
		case 'k':
			type = king;
			break;
		default:
			std::cout << "Unknown chess piece - WTF?" << std::endl;
			break;
	}


	return type;

}
