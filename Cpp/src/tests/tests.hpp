/*
 * tests.hpp
 *
 *  Created on: Sep 3, 2016
 *      Author: vabi
 */

#ifndef TESTS_TESTS_HPP_
#define TESTS_TESTS_HPP_

#include <lib/basics.hpp>

struct testResult {
	bool passed;
	std::string testName;
};

std::string getZeroPosition();
std::string getRandomPosition();
char getRandomPiece(playerColor color);
uint32_t getRandomField();
figureType getFigureTypeFromChar(char piece);
void runTests();
uint32_t perftNodes(chessPosition* c, uint16_t depth);

testResult testMakeMove();
testResult testUndoMove();
testResult perftTest();
#endif /* TESTS_TESTS_HPP_ */
