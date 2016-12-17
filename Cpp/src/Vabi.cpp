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
#include <fstream>
#include <DataTypes/threadSafeQueue.hpp>
#include <tr1/memory>
#include <lib/Attacks/attacks.hpp>
#include <lib/bitfiddling.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <Search/search.hpp>
#include <hashTables/hashTables.hpp>
#include <userInterface/UIlayer.hpp>
#include <sstream>
#include <tests/tests.hpp>
extern int32_t completePieceTables[7][2][64];
extern int16_t pieceTables[7][2][64];
extern int16_t endGamepieceTables[7][2][64];
extern int16_t figureValues[];



int main() {

	/*uint64_t typeArr[13] = {0};
	uint64_t captureArr[7] = {0};
	std::ifstream logger;
	logger.open("/home/vabi/log7.txt");
	std::string line;
	while(std::getline(logger, line)){

		std::vector<std::string> seglist;
		std::string part;
		std::stringstream sstream = std::stringstream(line);
		while(std::getline(sstream, part, ' '))
		{
		   seglist.push_back(part);
		}

		uint16_t type, captureType;
		std::stringstream(seglist[2]) >> type;
		std::stringstream(seglist[3]) >> captureType;

		typeArr[type]++;
		captureArr[captureType]++;


	}

	for(uint32_t ind=0; ind < 13; ind++){
		std::cout << typeArr[ind] << " ";
	}
	std::cout << std::endl;

	for(uint32_t ind=0; ind < 7; ind++){
		std::cout << captureArr[ind] << " ";
	}
	std::cout << std::endl;
	return 0;*/
	for(uint32_t index=0; index < 7; index++) {
		for(uint32_t t=0; t < 2; t++) {
			for(uint32_t k=0; k < 64; k++) {
				completePieceTables[index][t][k] = ((uint16_t) (pieceTables[index][t][k]/2+figureValues[index])) |  ( ((uint16_t) (endGamepieceTables[index][t][k]/2+figureValues[index])) << 16);

			}
		}
	}

	srand (time(NULL));
	fillZobristHash();
	/*std::ofstream out("/home/vabi/TeX/Chess/positions.tex");
	std::ifstream in("/home/vabi/code/chessEngine/chessGui/openingPositions.txt");
	std::string line;
	while(getline(in, line)){
		if(line.at(0) == '%'){
			continue;
		}
		latexOutput(line, out);
	}*/

	runPerformanceTests();
	return 0;
	/*std::cout << testPerftTestSuite().passed << std::endl;
	return 0;*/
	UIloop();
	//00000BNR00000PKQ00000NPR00000N0P000000000000000p0qpN0npnN000000kw0000
}

