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
	o.open("/home/vabi/bishopMoveTables.txt");

	o << generateBishopMoveTablesString();
	o.close();*/




	//::cout << generateBishopReachableFields() <<std::endl;
	//std::cout << generateRookMagicNumber(16) << std::endl;
	//std::cout << generateRookMagicNumbers() << std::endl;
	/*vdt_vector<vdt_vector<uint64_t>> vec =  generateBishopMoveTables();
	for(int k = 0; k < 1000000; k++) {
		uint64_t occupancy =  getRandUint64();
		uint32_t field = getRandomField();
		uint64_t blocker = occupancy & bishopFieldTable[field];

		uint64_t moves = generateBishopMoveTable(field, blocker);
		uint64_t magicNumber = bishopMagicNumbers[field];
		uint16_t hashValue = (blocker*magicNumber) >> 55;
		uint64_t magic_moves = bishopMoveTables[field][hashValue];

		/*outPutuint64(blocker);
		outPutuint64(moves);
		outPutuint64(magic_moves);*/

		/*if(moves-magic_moves) {
			std::cout << "Epic fail!" << std::endl;
		}
	}*/




	/*ofstream o;
	o.open("/home/vabi/rookMoveTables.txt");

	o << generateRookMoveTablesString();
	o.close();*/


	//std::cout << generateRookMagicNumbers() << std::endl;
	/*std::string a = generateRookReachableFields();
	std::cout << a <<std::endl;*/



	std::string position = "RNBQKBNRPPPPPPPP00000000000000000000000000000000pppppppprnbqkbnr";
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
	}
	/*testResult ret = testMakeMove();
	std::cout << ret.passed << std::endl;
	ret = testUndoMove();
	std::cout << ret.passed << std::endl;*/

	/*uint64_t blockers[1024];

	for (uint16_t ind=0; ind < 1024; ind++) {

		uint64_t blocker = 0;
		uint16_t cnt = 0;
		for(uint16_t i=0; i < 64; i++){
			if(ROOKE4 & (1UL << i)) {

				if(ind & (1 << cnt)){
					blocker = blocker | (1UL << i);
				}
				cnt++;
			}
		}

		blockers[ind] = blocker;


	}*/



/*	uint64_t numTries=0;
	while(!foundMagic) {
		numTries++;
		magicNumber = getRandUint64() & getRandUint64() & getRandUint64();
		bool indexCheck[1024] = {false};
		foundMagic = true;
		for(uint16_t ind=0; ind < 1024; ind++) {
			uint16_t val = ((blockers[ind]*magicNumber) >> 54);

			if(indexCheck[val] == true){
				foundMagic = false;
				break;
			}
			indexCheck[val] = true;

		}




	}
	std::cout << numTries    << std::endl;
	std::cout << magicNumber << std::endl;*/

	//uint64_t rookE4moveTable[1024];

	/*for(uint16_t ind=0; ind < 1024; ind++) {
		uint64_t hashValue = ((blockers[ind]*magicNumber) >> 54);

		uint64_t rookMoveTable = 0;
		int16_t field         = 28;
		int16_t f_copy        = field-8;
		while(f_copy >= 0){
			 rookMoveTable =  rookMoveTable | BIT64(f_copy);
			 if(BIT64(f_copy) & blockers[ind]){
				 break;
			 }
			 f_copy = f_copy-8;
		}
		f_copy = field+8;
		while(f_copy < 64){
			 rookMoveTable =  rookMoveTable | BIT64(f_copy);
			 if(BIT64(f_copy) & blockers[ind]){
				 break;
			 }
			 f_copy = f_copy+8;
		}
		f_copy = field+1;
		while( (f_copy >> 3) == (field >> 3)) {
			rookMoveTable =  rookMoveTable | BIT64(f_copy);
			 if(BIT64(f_copy) & blockers[ind]){
				 break;
			 }
			f_copy = f_copy+1;
		}
		f_copy = field-1;
		if(f_copy >= 0) {
			while( (f_copy >> 3) == (field >> 3)) {
				rookMoveTable =  rookMoveTable | BIT64(f_copy);
				 if(BIT64(f_copy) & blockers[ind]){
					 break;
				 }
				f_copy = f_copy-1;
			}
		}

		rookE4moveTable[hashValue] = rookMoveTable;
	}

	uint64_t occupancy = getRandUint64();
	outPutuint64(occupancy);
	uint16_t hashValue = ((occupancy & ROOKE4)*magicNumber)  >> 54;
	std::cout << hashValue << std::endl;
	uint64_t rookMoves =  rookE4moveTable[hashValue];
	outPutuint64(rookMoves);*/

}
