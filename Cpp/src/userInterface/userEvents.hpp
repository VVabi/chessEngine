/*
 * userEvents.hpp
 *
 *  Created on: Mar 9, 2017
 *      Author: vabi
 */

#ifndef USEREVENTS_HPP_
#define USEREVENTS_HPP_
#include <string>
#include <list>

enum uciInput {invalid, uci, debug, isready, setoption, ucinewgame, position, go, stop, ponderhit, quit, perft, eval, pawnEval, see, clear, info, setEvalParam, checkquiet, performanceTests, getMoveOrdering};

struct userEvent {
 public:
    uciInput input;
    std::list<std::string> data;

    userEvent(uciInput in, std::list<std::string> d) {
        input = in;
        data = d;
    }
};

userEvent getNextUserEvent();
void initUserEvents();
#endif /* USEREVENTS_HPP_ */
