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
#include <tests/tests.hpp>
#include <DataTypes/vdt_vector.hpp>
#include <lib/moveGeneration/moveGeneration.hpp>
#include <magicNumberGeneration/magicNumberGeneration.hpp>
#include <fstream>
#include <DataTypes/threadSafeQueue.hpp>
#include <tr1/memory>
#include <communication/Network/message.h>
#include <communication/gen/VMPmessages.h>
#define ROOKE4 ((1UL << 25) | (1UL << 26) | (1UL << 27) | (1UL << 29) | (1UL << 30) | (1UL << 12) | (1UL << 20) | (1UL << 36) | (1UL << 44) | (1UL << 52))

extern const uint64_t rookFieldTable[];
extern const uint64_t bishopFieldTable[];
extern const uint64_t rookMoveTables[64][4096];
extern const uint64_t bishopMoveTables[64][512];
extern const uint64_t rookMagicNumbers[];
extern const uint64_t bishopMagicNumbers[];

void outPutuint64(uint64_t num){

	for	(int j=7; j >= 0; j--)	{
		for(int i=0; i < 8; i++){
			uint64_t toCheck = (1UL << (8*j+i));
			if(num & toCheck) {
				std::cout << "1";
			}
			else{
				std::cout << "0";
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;


}

int main() {




	/*ofstream o;
	o.open("/home/vabi/rookMoveTables.txt");

	o << generateRookMoveTablesString();
	o.close();*/






	/*std::string position = "RNBQKBNRPPPPPPPP00000000000000000000000000000000pppppppprnbqkbnr";
	chessPosition c = stringToChessPosition(position);
	while(1){

		std::string output = chessPositionToOutputString(c);
		std::cout << output << std::endl;
		vdt_vector<chessMove> moves = vdt_vector<chessMove>(100);

		std::string nextMove;
		cin >> nextMove;
		uint64_t mv = stringToMove(nextMove);
		generateAllMoves(&moves, &c);

		bool found = false;
		chessMove m;
		for(uint16_t ind=0; ind < moves.length; ind++) {
			if(moves[ind].move == mv){
				found = true;
				m = moves[ind];
			}


		}


		if(found){
			std::cout << moveToString(m, c) << std::endl;
			makeMove(&m, &c);

		} else {
			std::cout << "Invalid move!" << std::endl;
		}
		moves.free_array();
	}*/





}
