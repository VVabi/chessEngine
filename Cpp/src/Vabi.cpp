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
#include <parameters/parameters.hpp>

extern int32_t completePieceTables[7][2][64];
extern int16_t pieceTables[7][2][64];
extern int16_t endGamepieceTables[7][2][64];
extern int16_t rawPieceTables[6][32];
extern int16_t endgameRawPieceTables[6][32];

int main() {

	initializeParameters();

	for(uint16_t type = 0; type < 6; type++) {

		for(uint16_t field = 0; field < 32; field++) {

			int16_t value = rawPieceTables[type][field];
			uint16_t file = field % 4;
			uint16_t row = field/4;
			pieceTables[type][white][8*row+file] = value;
			pieceTables[type][white][8*row+7-file] = value;
			pieceTables[type][black][8*(7-row)+file] = value;
			pieceTables[type][black][8*(7-row)+7-file] = value;


		}

	}

	for(uint16_t type = 0; type < 6; type++) {

		for(uint16_t field = 0; field < 32; field++) {

			int16_t value = endgameRawPieceTables[type][field];
			uint16_t file = field % 4;
			uint16_t row = field/4;
			endGamepieceTables[type][white][8*row+file] = value;
			endGamepieceTables[type][white][8*row+7-file] = value;
			endGamepieceTables[type][black][8*(7-row)+file] = value;
			endGamepieceTables[type][black][8*(7-row)+7-file] = value;
		}

	}


	for(uint16_t ind=0; ind < 2; ind++) {
		for(uint16_t field=0; field < 64; field++) {
			endGamepieceTables[6][ind][field] = 0;
			pieceTables[6][ind][field] = 0;
		}
	}

	const evalParameters* evalPars =  getEvalParameters();
	for(uint32_t index=0; index < 7; index++) {
		for(uint32_t t=0; t < 2; t++) {
			for(uint32_t k=0; k < 64; k++) {
				completePieceTables[index][t][k] = ((uint16_t) (pieceTables[index][t][k]+evalPars->figureValues[index])) |  ( ((uint16_t) (endGamepieceTables[index][t][k]+evalPars->figureValues[index])) << 16);
			}
		}
	}


	//std::cout << "ENDGAME" << std::endl;
	for(uint32_t index=0; index < 7; index++) {
			for(uint32_t t=0; t < 2; t++) {
				for(uint32_t k=0; k < 64; k++) {
						int16_t val1 = endGamepieceTables[index][t][k];
						uint16_t file = FILE(k);
						uint16_t row = ROW(k);
						uint16_t mirrorFile = 7-file;
						uint16_t field = 8*row+mirrorFile;
						int16_t val2 = endGamepieceTables[index][t][field];

						uint16_t bfield = (7-row)*8+file;
						int16_t val3 = endGamepieceTables[index][1-t][bfield];

						if(val1 != val2) {
							std::cout << "East/west" << std::endl;
							std::cout << index << " " << t << " " << k << std::endl;
						}

						if(val1 != val3) {
							std::cout << "North/South" << std::endl;
							std::cout << index << " " << t << " " << k << std::endl;
						}
				}
			}
	}
	//std::cout << "MIDGAME" << std::endl;
	for(uint32_t index=0; index < 7; index++) {
			for(uint32_t t=0; t < 2; t++) {
				for(uint32_t k=0; k < 64; k++) {
						int16_t val1 = pieceTables[index][t][k];
						uint16_t file = FILE(k);
						uint16_t row = ROW(k);
						file = 7-file;
						uint16_t field = 8*row+file;
						int16_t val2 = pieceTables[index][t][field];
						uint16_t bfield = (7-row)*8+file;
						int16_t val3 = pieceTables[index][1-t][bfield];
						if(val1 != val2) {
							std::cout << "East/west" << std::endl;
							std::cout << index << " " << t << " " << k << std::endl;
						}

						if(val1 != val3) {
							std::cout << "North/South" << std::endl;
							std::cout << index << " " << t << " " << k << std::endl;
						}
				}
			}
	}

	srand (get_timestamp());
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

/*
	runPerformanceTests();
	return 0;*/
	/*std::cout << testPerftTestSuite().passed << std::endl;
	return 0;*/
	UIloop();
	//00000BNR00000PKQ00000NPR00000N0P000000000000000p0qpN0npnN000000kw0000
}

