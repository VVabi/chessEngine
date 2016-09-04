/*
 * tests.hpp
 *
 *  Created on: Sep 3, 2016
 *      Author: vabi
 */

#ifndef TESTS_TESTS_HPP_
#define TESTS_TESTS_HPP_



struct testResult {
	bool passed;
};

std::string getZeroPosition();
std::string getRandomPosition();
char getRandomPiece(playerColor color);
uint32_t getRandomField();
figureType getFigureTypeFromChar(char piece);


testResult testMakeMove();
testResult testUndoMove();

#endif /* TESTS_TESTS_HPP_ */
