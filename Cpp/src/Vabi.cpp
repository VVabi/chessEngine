//============================================================================
// Name        : Vabi.cpp
// Author      : Fabian Lenhardt
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;
#include <lib/basics.hpp>
#include <lib/moveMaking/moveMaking.hpp>

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	//std::string position = "RNBQKBNRPPPPPPPP00000000000000000000000000000000pppppppprnbqkbnr";
	std::string position = "R0000000000000000000000000000000000000000000000000000000r0000000";
	chessPosition c = stringToChessPosition(position);
	std::string res = chessPositionToString(c);
	std::cout << res << std::endl;
	chessMove move;
	move.captureType = rook;
	move.type        = rookMove;
	move.move        = BIT64(0) |  BIT64(56);

	makeMove(&move, &c);
	res = chessPositionToString(c);
	std::cout << res << std::endl;

	return 0;
}
