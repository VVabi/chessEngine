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


#define ROOKE4 ((1UL << 25) | (1UL << 26) | (1UL << 27) | (1UL << 29) | (1UL << 30) | (1UL << 12) | (1UL << 20) | (1UL << 36) | (1UL << 44) | (1UL << 52))


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
	/*vdt_vector<chessMove> moves = vdt_vector<chessMove>(5);
	std::string position = "RNBQKBNRPPPPPPPP000000000000000000000000PPNNRRQQpppppppprnbqkbnr";
	chessPosition c = stringToChessPosition(position);
	c.toMove = black;
	generateAllMoves(&moves, &c);

	std::cout << moves.length << std::endl;

	for(uint16_t ind=0; ind < moves.length; ind++){
		std::cout << moveToString(moves[ind], c) << std::endl;

	}*/
	/*testResult ret = testMakeMove();
	std::cout << ret.passed << std::endl;
	ret = testUndoMove();
	std::cout << ret.passed << std::endl;*/
	outPutuint64(ROOKE4);
	uint64_t blockers[1024];

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


	}

	bool foundMagic = false;
	uint64_t magicNumber = 18295946501160996;

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

	uint64_t rookE4moveTable[1024];

	for(uint16_t ind=0; ind < 1024; ind++) {
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
	outPutuint64(rookMoves);

}
