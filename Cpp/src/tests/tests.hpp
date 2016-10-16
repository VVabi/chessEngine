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
std::string getRandomPosition(playerColor color);
char getRandomPiece(playerColor color);
uint32_t getRandomField();
figureType getFigureTypeFromChar(char piece);
void runTests();
uint64_t perftNodes(chessPosition* c, uint16_t depth);

testResult testMakeMove();
testResult testUndoMove();
testResult testPerftTestSuite();
#endif /* TESTS_TESTS_HPP_ */
