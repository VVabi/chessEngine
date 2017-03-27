/*
 * history.cpp
 *
 *  Created on: Oct 16, 2016
 *      Author: vabi
 */



#include <stdint.h>
#include <iostream>



int32_t historyTable[2][64][64];

void clearHistoryTable() {

	for(uint16_t color=0; color < 2; color++){
		for(uint16_t from=0; from < 64; from++){
			for(uint16_t to=0; to<64; to++){
				historyTable[color][from][to] = 0;
			}
		}
	}


}

void rescaleHistoryTable(){
//	std::cout << "Rescaling history table" << std::endl;
	for(uint16_t color=0; color < 2; color++){
		for(uint16_t from=0; from < 64; from++){
			for(uint16_t to=0; to<64; to++){
				historyTable[color][from][to] = historyTable[color][from][to]/4;
			}
		}
	}
}


