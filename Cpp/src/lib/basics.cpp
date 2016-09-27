/*
 * basics.cpp
 *
 *  Created on: Sep 1, 2016
 *      Author: vabi
 */


#include "basics.hpp"
#include <iostream>
#include <string.h>
#include <lib/bitfiddling.h>
#include <lib/Defines/boardParts.hpp>
#include <hashTables/hashTables.hpp>
#include <lib/DebugFunctions/debugFunctions.hpp>

static char figureNames[2][6] = {{'P', 'N', 'B', 'R', 'Q', 'K'},
		{'p', 'n', 'b', 'r', 'q', 'k'},
};


inline figureType toFigureType(uint16_t num){
#ifdef DEBUG

	if(num > 5) {
		std::cout << "There are only 6 different figuretypes in chess..." << std::endl;
		while(1) {

		}

	}
#endif

	return (figureType) num;

}

std::string chessPositionToString(chessPosition position) {
	//Not performance-critical
	//---------------------------
	std::string ret = "";

	for (uint8_t ind=0; ind < 64; ind++) {
		uint64_t num = (1UL << ind);
		bool found = false;
		for	(uint8_t color=0; color < 2; color++) {
			for	(uint8_t figureCnt = 0; figureCnt < 6; figureCnt++) {

				if (position.pieceTables[color][figureCnt] & num) {
					ret.push_back(figureNames[color][figureCnt]);
					found = true;
				}

			}
		}

		if (!found) {
			ret.push_back('0');
		}
	}

	if(position.toMove == white) {
		ret.push_back('w');
	} else {
		ret.push_back('b');
	}

	uint8_t castlingRights = position.castlingRights;

	if(castlingRights & 1) {
		ret.push_back('K');
	} else {
		ret.push_back('0');
	}

	if(castlingRights & 2) {
		ret.push_back('Q');
	} else {
		ret.push_back('0');
	}

	if(castlingRights & 4) {
		ret.push_back('k');
	} else {
		ret.push_back('0');
	}

	if(castlingRights & 8) {
		ret.push_back('q');
	} else {
		ret.push_back('0');
	}


	return ret;
}

std::string chessPositionToOutputString(chessPosition position){
	//Not performance-critical
	//---------------------------
	std::string ret = "";

	for (int8_t row=7; row >=0; row--) {
		for (int8_t column=0; column < 8; column++) {
		uint64_t num = (1UL << (8*row+column));
		bool found = false;
		for	(uint8_t color=0; color < 2; color++) {
			for	(uint8_t figureCnt = 0; figureCnt < 6; figureCnt++) {

				if (position.pieceTables[color][figureCnt] & num) {
					ret.push_back(figureNames[color][figureCnt]);
					found = true;
				}

			}
		}

		if (!found) {
			ret.push_back('0');
		}
		}
		ret.push_back('\n');
	}
	return ret;



}

void zeroInitPosition(chessPosition* position) {
#ifdef DEBUG
	position->toMove = white;
	position->pieces = vdt_vector<uint64_t>(2);
	uint64_t dummy = 0;
	position->pieces.add(&dummy);
	position->pieces.add(&dummy);

	vdt_vector<uint64_t> whiteTable = vdt_vector<uint64_t>(NUM_DIFFERENT_PIECES+1);
	whiteTable.add(&dummy);
	whiteTable.add(&dummy);
	whiteTable.add(&dummy);
	whiteTable.add(&dummy);
	whiteTable.add(&dummy);
	whiteTable.add(&dummy);
	whiteTable.add(&dummy);

	vdt_vector<uint64_t> blackTable = vdt_vector<uint64_t>(NUM_DIFFERENT_PIECES+1);
	blackTable.add(&dummy);
	blackTable.add(&dummy);
	blackTable.add(&dummy);
	blackTable.add(&dummy);
	blackTable.add(&dummy);
	blackTable.add(&dummy);
	blackTable.add(&dummy);

	position->pieceTables =  vdt_vector<vdt_vector<uint64_t>>(2);
	position->pieceTables.add(&whiteTable);
	position->pieceTables.add(&blackTable);

	position->enPassantFile = 0;
	position->madeMoves = vdt_vector<chessMove>(300);
	position->castlingAndEpStack = vdt_vector<uint16_t>(300);
	position->figureEval    = 0;
#else
	memset(position, 0, sizeof(chessPosition));
	position->madeMoves = vdt_vector<chessMove>(300);
	position->castlingAndEpStack = vdt_vector<uint16_t>(300);
#endif
	position->zobristHash    = calcZobristHash(position);


}

chessPosition stringToChessPosition(std::string strposition) {
	//Not performance-critical
	//---------------------------
	chessPosition position;
	zeroInitPosition(&position);

	if (strposition.length() != 69) {
		std::cout << "Invalid chess position string!!" << std::endl;
		return position;
	}

	for (uint8_t ind=0; ind < 64; ind++) {
		char c = strposition.at(ind);
			switch(c){
				case 'K':
					position.pieces[white] 					|= (1UL << ind);
					position.pieceTables[white][king] 		|= (1UL << ind);
					break;
				case 'Q':
					position.pieceTables[white][queen] 		|= (1UL << ind);
					position.pieces[white] 					|= (1UL << ind);
					break;
				case 'R':
					position.pieceTables[white][rook] 		|= (1UL << ind);
					position.pieces[white] 					|= (1UL << ind);
					break;
				case 'B':
					position.pieceTables[white][bishop] 	|= (1UL << ind);
					position.pieces[white] 					|= (1UL << ind);
					break;
				case 'N':
					position.pieceTables[white][knight] 	|= (1UL << ind);
					position.pieces[white] 					|= (1UL << ind);
					break;
				case 'P':
					position.pieceTables[white][pawn] 		|= (1UL << ind);
					position.pieces[white] 					|= (1UL << ind);
					break;
				case 'k':
					position.pieceTables[black][king] 		|= (1UL << ind);
					position.pieces[black] 					|= (1UL << ind);
					break;
				case 'q':
					position.pieceTables[black][queen]  	|= (1UL << ind);
					position.pieces[black] 					|= (1UL << ind);
					break;
				case 'r':
					position.pieceTables[black][rook]  		|= (1UL << ind);
					position.pieces[black] 					|= (1UL << ind);
					break;
				case 'b':
					position.pieceTables[black][bishop]  	|= (1UL << ind);
					position.pieces[black] 					|= (1UL << ind);
					break;
				case 'n':
					position.pieceTables[black][knight]  	|= (1UL << ind);
					position.pieces[black] 					|= (1UL << ind);
					break;
				case 'p':
					position.pieceTables[black][pawn]  		|= (1UL << ind);
					position.pieces[black] 					|= (1UL << ind);
					break;
				case '0':
					break;
				default:
					std::cout << "invalid position string" << std::endl;
					break;
			}


	}

	if(strposition.at(64) == 'w') {
		position.toMove = white;
	} else if(strposition.at(64) == 'b') {
		position.toMove = black;
	} else {
		std::cout << "invalid position string" << std::endl;
	}

	position.castlingRights = 0;
	if (strposition.at(65) == 'K') {
		position.castlingRights = position.castlingRights | 1;
	}
	if (strposition.at(66) == 'Q') {
		position.castlingRights = position.castlingRights | 2;
	}
	if (strposition.at(67) == 'k') {
		position.castlingRights = position.castlingRights | 4;
	}
	if (strposition.at(68) == 'q') {
		position.castlingRights = position.castlingRights | 8;
	}

	position.figureEval   = calcFigureEvaluation(&position);
	position.pieceTableEval = ((1 << 15) + position.figureEval+ calcPieceTableValue(&position))
							+(((1 << 14) + calcEndGamePieceTableValue(&position)+position.figureEval) << 16);
	position.zobristHash    = calcZobristHash(&position);

	return position;
}


std::string moveToString(chessMove move, chessPosition position) {

	uint64_t mv = move.move;
	uint64_t occupancy = position.pieces[position.toMove];
	uint64_t source = mv & occupancy;
	uint64_t target = mv^source;

	uint16_t sourceField = findLSB(source);
	uint16_t targetField = findLSB(target);

	char ret[4];
	ret[0] = FILE(sourceField)+'a';
	ret[1] = ROW(sourceField)+'1';
	ret[2] = FILE(targetField)+'a';
	ret[3] = ROW(targetField)+'1';

	std::string retString = "";
	retString.push_back(ret[0]);
	retString.push_back(ret[1]);
	retString.push_back(ret[2]);
	retString.push_back(ret[3]);
	return retString;

}

uint64_t stringToMove(std::string mv){
	char sourceColumn = mv[0]-'a';
	char sourceRow = mv[1]-'1';
	uint16_t sourceField = sourceRow*8+sourceColumn;
	char targetColumn = mv[2]-'a';
	char targetRow = mv[3]-'1';
	uint16_t targetField = targetRow*8+targetColumn;

	return BIT64(sourceField) | BIT64(targetField);
}


void debug_incremental_calculations(chessPosition* position) {

	int16_t eval = calcFigureEvaluation(position);

	if(eval != position->figureEval){
		std::cout << "Figure evaluation is wrong after moveMake!" << std::endl;
	}

	int16_t pieceTableEval = calcPieceTableValue(position)+eval;
	int32_t buffer =  position->pieceTableEval & 0xFFFF;
	int32_t buffer2  = buffer-(1 << 15);

	if(pieceTableEval !=  buffer2){
		std::cout << "Piece table evaluation is wrong after moveMake!" << std::endl;
	}

	int16_t endgamepieceTableEval = calcEndGamePieceTableValue(position)+eval;
	buffer =  (position->pieceTableEval >> 16);
	buffer2  = buffer-(1 << 14);
	if(endgamepieceTableEval !=  buffer2){
		std::cout << "endgame Piece table evaluation is wrong after moveMake!" << std::endl;
	}

	uint64_t hash = calcZobristHash(position);
	if(hash != position->zobristHash){
		std::cout << "zobrist hash wrong after make move" << std::endl;
	}



}

