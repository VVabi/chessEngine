/*
 * testMoveMaking.cpp
 *
 *  Created on: Sep 3, 2016
 *      Author: vabi
 */


#include <lib/basics.hpp>
#include <lib/moveMaking/moveMaking.hpp>
#include "tests.hpp"

#include <iostream>

testResult testMakeMove(){
	testResult ret;
	ret.passed = true;

	/*This tests tests moves which are not legal chessmoves. However, the makemove function has to correctly execute the moves nevertheless.
	 */

	//non-captures
	//----------------
	for(uint8_t k=0; k < 2; k++) {

		playerColor color = (playerColor) k;

		for	(uint32_t ind = 0; ind < 1000; ind++) {
			std::string randomPosition = getZeroPosition();

			uint32_t start = getRandomField();
			uint32_t end   = getRandomField();
			char piece     = getRandomPiece(color);
			randomPosition[start] = piece;
			randomPosition[end]   = '0';
			chessPosition pos = stringToChessPosition(randomPosition);
			pos.toMove = color;
			chessMove move;
			move.move 		 = BIT64(start) | BIT64(end);
			move.type 		 = (moveType) getFigureTypeFromChar(piece);
			move.captureType = none;
			makeMove(&move, &pos);
			std::string newPosition = chessPositionToString(pos);

			randomPosition[start] = '0';
			randomPosition[end]   = piece;

			if (randomPosition.compare(newPosition)) {
				std::cout << "move making error!" << std::endl;
				ret.passed = false;
			}

		}
	}

	//captures
	//----------------
	for(uint8_t k=0; k < 2; k++) {

		playerColor color = (playerColor) k;

		for	(uint32_t ind = 0; ind < 1000; ind++) {
			std::string randomPosition = getZeroPosition();

			uint32_t start = getRandomField();
			uint32_t end   = getRandomField();
			char piece     = getRandomPiece(color);
			char capturePiece = getRandomPiece((playerColor) (1-color));
			randomPosition[start] = piece;
			randomPosition[end]   = capturePiece;
			chessPosition pos = stringToChessPosition(randomPosition);
			pos.toMove = color;
			chessMove move;
			move.move 		 = BIT64(start) | BIT64(end);
			move.type 		 = (moveType) getFigureTypeFromChar(piece);
			move.captureType = getFigureTypeFromChar(capturePiece);
			makeMove(&move, &pos);
			std::string newPosition = chessPositionToString(pos);

			randomPosition[start] = '0';
			randomPosition[end]   = piece;

			if (randomPosition.compare(newPosition)) {
				std::cout << "move making error!" << std::endl;
				ret.passed = false;
			}

		}
	}


	return ret;

}



testResult testUndoMove() {
	testResult ret;
	ret.passed = true;

	/*This tests tests moves which are not legal chessmoves. However, the makemove function has to correctly execute the moves nevertheless.
	 */

	//non-captures
	//----------------
	for(uint8_t k=0; k < 2; k++) {

		playerColor color = (playerColor) k;

		for	(uint32_t ind = 0; ind < 1000; ind++) {
			std::string randomPosition = getZeroPosition();

			uint32_t start = getRandomField();
			uint32_t end   = getRandomField();
			char piece     = getRandomPiece(color);
			randomPosition[start] = piece;
			randomPosition[end]   = '0';
			chessPosition pos = stringToChessPosition(randomPosition);
			pos.toMove = color;
			chessMove move;
			move.move 		 = BIT64(start) | BIT64(end);
			move.type 		 = (moveType) getFigureTypeFromChar(piece);
			move.captureType = none;
			makeMove(&move, &pos);
			undoMove(&move, &pos);
			std::string newPosition = chessPositionToString(pos);



			if (randomPosition.compare(newPosition)) {
				std::cout << "move making error!" << std::endl;
				ret.passed = false;
			}

		}
	}

	//captures
	//----------------
	for(uint8_t k=0; k < 2; k++) {

		playerColor color = (playerColor) k;

		for	(uint32_t ind = 0; ind < 1000; ind++) {
			std::string randomPosition = getZeroPosition();

			uint32_t start = getRandomField();
			uint32_t end   = getRandomField();
			char piece     = getRandomPiece(color);
			char capturePiece = getRandomPiece((playerColor) (1-color));
			randomPosition[start] = piece;
			randomPosition[end]   = capturePiece;
			chessPosition pos = stringToChessPosition(randomPosition);
			pos.toMove = color;
			chessMove move;
			move.move 		 = BIT64(start) | BIT64(end);
			move.type 		 = (moveType) getFigureTypeFromChar(piece);
			move.captureType = getFigureTypeFromChar(capturePiece);
			makeMove(&move, &pos);
			undoMove(&move, &pos);
			std::string newPosition = chessPositionToString(pos);



			if (randomPosition.compare(newPosition)) {
				std::cout << "move making error!" << std::endl;
				ret.passed = false;
			}

		}
	}


	return ret;
}


