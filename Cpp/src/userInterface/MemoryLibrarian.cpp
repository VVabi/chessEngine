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

static std::string currentPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
static std::vector<std::string> moveList;


void memoryLibrarianAdd(std::string newPosition, std::vector<std::string> moves) {
    moveList = moves;
    currentPosition = newPosition;
}

chessPosition memoryLibrarianRetrievePosition() {
    chessPosition cposition = FENtoChessPosition(currentPosition);
    for(std::string seg: moveList) {
        //std::cout << seg << std::endl;
        if(!checkAndMakeMove(&cposition, seg)) {
            break;
        }
    }
    return cposition;
}




