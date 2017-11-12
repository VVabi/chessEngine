/*
 * MemoryLibrarian.cpp
 *
 *  Created on: Mar 11, 2017
 *      Author: vabi
 */


#include <string>
#include <vector>
#include <lib/basics.hpp>
#include <userInterface/UIlayer.hpp>
#include <fstream>
#include <iostream>

static std::string currentPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
static std::vector<std::string> moveList;


void memoryLibrarianAdd(std::string newPosition, std::vector<std::string> moves) {
    moveList = moves;
    currentPosition = newPosition;
}

//uint32_t cnt = 0;
//std::ofstream positions("/home/vabi/positions.txt");

chessPosition memoryLibrarianRetrievePosition() {
    //cnt++;



    chessPosition cposition = FENtoChessPosition(currentPosition);
    for (std::string seg : moveList) {
        //std::cout << seg << std::endl;
        if (!checkAndMakeMove(&cposition, seg)) {
            break;
        }
    }

    /*if (cnt > 10) {
       positions << chessPositionToFenString(cposition) << std::endl;
    }*/
    return cposition;
}




