/*
 * selfPlaying.cpp
 *
 *  Created on: Sep 25, 2016
 *      Author: vabi
 */

#include "selfPlaying.hpp"
#include <lib/bitfiddling.h>
#include <lib/moveGeneration/moveGeneration.hpp>
#include <lib/moveMaking/moveMaking.hpp>
#include <Search/search.hpp>
#include <lib/Evaluation/evaluation.hpp>
#include <userInterface/UIlayer.hpp>

bool enablePawnEval;

/*int32_t selfPlayMakeMove(chessPosition* position, uint16_t whitedepth, uint16_t blackdepth) {



    uint16_t depth = 0;
    if (position->toMove == white) {
        depth = whitedepth;
    } else {
        depth = blackdepth;
    }
    chessMove bestMove;
    int32_t eval = negamax(position, 0, depth+3, depth, -32000, 32000, &bestMove);
    if (position->toMove == black) {
        eval = -eval;
    }
    //std::cout << moveToString(bestMove, *position) << std::endl;
    makeMove(&bestMove, position);


    return eval;


}

int16_t playSingleGameSelf(uint16_t depth1, uint16_t depth2, bool switchColors) {
    std::string positionStr = "RNBQKBNRPPPPPPPP00000000000000000000000000000000pppppppprnbqkbnrwKQkq";
    chessPosition position = stringToChessPosition(positionStr);

    uint16_t whitedepth, blackdepth;

    if (switchColors) {
        whitedepth = depth2;
        blackdepth = depth1;
    } else {
        whitedepth = depth1;
        blackdepth = depth2;
    }

    int16_t ret = 0;
    while (true) {
        enablePawnEval      = !enablePawnEval;
        int32_t currentEval = selfPlayMakeMove(&position, whitedepth, blackdepth);


        if (currentEval > 90000) {
            ret = 1;
            break;
        }

        if (currentEval < -90000) {
            ret = -1;
            break;
        }

        if (position.madeMoves.length > 200) {
            std::cout << "Eval " << currentEval << std::endl;
            if (currentEval > 300) {
                ret = 1;
            } else if (currentEval < -300) {
                ret = -1;
            } else {
                ret = 0;
            }
            break;
        }


    }


    if (switchColors) {
        ret = -ret;
    }
    position.dataStack.free_array();
    position.madeMoves.free_array();
    std::cout << ret << std::endl;
    return ret;
}



selfPlayResult playSelf(uint16_t depth1, uint16_t depth2, uint32_t numGames) {
    selfPlayResult ret;
    ret.numGames = numGames;
    ret.engine1Wins = 0;
    ret.engine2Wins = 0;
    ret.draws       = 0;

    bool switchColors = false;


    for (uint32_t ind = 0; ind <numGames; ind++) {
        enablePawnEval = switchColors;
        std::cout << "GAME " << ind << std::endl;
        int16_t result = playSingleGameSelf(depth1, depth2, switchColors);
        switchColors = !switchColors;
        if (result == 1) {
            ret.engine1Wins++;
        }
        if (result == -1) {
            ret.engine2Wins++;
        }
        if (result == 0) {
            ret.draws++;
        }
    }




    return ret;


}*/

