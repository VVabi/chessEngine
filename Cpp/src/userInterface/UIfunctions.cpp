/*
 * UIfunctions.cpp
 *
 *  Created on: Oct 9, 2016
 *      Author: vabi
 */

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <lib/basics.hpp>
#include <fstream>
#include <lib/bitfiddling.h>
#include <hashTables/hashTables.hpp>
#include <lib/DebugFunctions/debugFunctions.hpp>
#include "UIlayer.hpp"
#include <Search/search.hpp>
#include <lib/Attacks/attacks.hpp>
#include <lib/moveMaking/moveMaking.hpp>
#include <lib/moveGeneration/moveGeneration.hpp>
#include <algorithm>
#include <lib/Evaluation/evaluation.hpp>
#include <atomic>

static char figureNames[2][6] = {{'P', 'N', 'B', 'R', 'Q', 'K'},
		{'p', 'n', 'b', 'r', 'q', 'k'},
};







uint8_t searchId = 0;

uint64_t get_timestamp(){
	struct timeval start;
	uint64_t mtime, seconds, useconds;
	gettimeofday(&start, NULL);
	seconds  = start.tv_sec;
	useconds = start.tv_usec;

	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
	return mtime;
}


uint64_t runSinglePositionPerformanceTest(std::string position, uint16_t depth, uint64_t* negamaxNodes, uint64_t* qNodes, bool useAspiration) {
	searchId++;
	chessPosition c = stringToChessPosition(position);

	resetSearchData();
	resetQuiescenceNodes();
	struct timeval start, end;
	//long mtime, seconds, useconds;
	gettimeofday(&start, NULL);
	//int32_t eval = negamax(&c, depth, -100000, 100000, &bestMove);
	int16_t alpha = -32000;
	int16_t beta  = 32000;
	int32_t searchdepth = 3;
	uint64_t start_ts = get_timestamp();
	setTotalTime(100000000, start_ts);
	while(searchdepth <= depth && (searchdepth < 14)) {

		bool succeeded = false;
		pvLine line;
		int32_t eval = negamax(&c, 0, depth+7, searchdepth, alpha, beta, &line);

		if(useAspiration) {
			if ((eval <= alpha)) {
				alpha = alpha-100;
			} else if(eval >= beta){
				beta = beta+100;
			}

			else {
				succeeded = true;
			}


		} else {
			succeeded = true;
		}

		if(succeeded){
			searchdepth++;
			alpha = eval-50;
			beta  = eval+50;
		}
	}
	gettimeofday(&end, NULL);

	/*seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;

	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;*/
	searchDebugData data = getSearchData();
	uint64_t nodeCount = getQuiescenceNodes()+data.totalNodes;
	*negamaxNodes = data.totalNodes; //getNodes();
	*qNodes       = getQuiescenceNodes();
	//double nps = ((double) nodeCount)/((double) mtime)*1000.0;
	/*std::cout << "Searched " <<  nodeCount << " positions in " << mtime << " for " << nps << " nodes per second" << std::endl;
	std::cout << "Forced move " << moveToString(bestMove, c) << std::endl;
	std::cout << "Evaluation "  << eval << std::endl;*/

	return nodeCount;

}



void outputTestResults() { //TODO: integrate properly so that we do not need the files
	std::ifstream with("/home/vabi/deltapruning2.txt");
	std::ifstream without("/home/vabi/deltapruning.txt");

	int withAsp, withoutAsp;
	int totalWith = 0;
	int totalWithout = 0;
	while(with >> withAsp){
		without >> withoutAsp;
		totalWith = totalWith+withAsp;
		totalWithout = totalWithout+withoutAsp;
		double ratio = ((double) withAsp)/((double) withoutAsp);
		std::cout << withAsp << " " << withoutAsp << " " << ratio << std::endl;

	}

	double ratio = ((double) totalWith)/((double) totalWithout);
	std::cout << totalWith << " " << totalWithout << " " << ratio << std::endl;


}

std::string chessPositionToString(const chessPosition position) {
	//Not performance-critical
	//---------------------------
	std::string ret = "";

	for (uint8_t ind=0; ind < 64; ind++) {
		uint64_t num = (1ULL << ind);
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

	uint8_t castlingRights = position.data.castlingRights;

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

std::string chessPositionToOutputString(const chessPosition position){
	//Not performance-critical
	//---------------------------
	std::string ret = "";

	for (int8_t row=7; row >=0; row--) {
		for (int8_t column=0; column < 8; column++) {
		uint64_t num = (1ULL << (8*row+column));
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

extern uint16_t repetitionData[16384];

chessPosition FENtoChessPosition(std::string fen){
	memset(repetitionData, 0, 16384*sizeof(uint16_t)); //TODO: this is a hack
	//Not performance-critical
	//---------------------------
	chessPosition position;
	zeroInitPosition(&position);

	//r1bqkb1r/pp3ppp/2np1n2/1N2p3/4P3/2N5/PPP2PPP/R1BQKB1R w KQkq - 0 7

	uint16_t ind=0;
	uint16_t row = 7;
	uint16_t file = 0;
	while(fen.at(ind) != ' ') {
		char current = fen.at(ind);

		if(current == '/'){
			row--;
			file = 0;
			ind++;
			continue;
		}

		if((current >= '0') && (current <='8')){
			uint8_t num = current-'0';
			file = file+num;
		} else {
			uint16_t field = 8*row+file;
			file++;
			switch(current){
				case 'K':
					position.pieces[white] 					|= (1ULL << field);
					position.pieceTables[white][king] 		|= (1ULL << field);
					break;
				case 'Q':
					position.pieceTables[white][queen] 		|= (1ULL << field);
					position.pieces[white] 					|= (1ULL << field);
					break;
				case 'R':
					position.pieceTables[white][rook] 		|= (1ULL << field);
					position.pieces[white] 					|= (1ULL << field);
					break;
				case 'B':
					position.pieceTables[white][bishop] 	|= (1ULL << field);
					position.pieces[white] 					|= (1ULL << field);
					break;
				case 'N':
					position.pieceTables[white][knight] 	|= (1ULL << field);
					position.pieces[white] 					|= (1ULL << field);
					break;
				case 'P':
					position.pieceTables[white][pawn] 		|= (1ULL << field);
					position.pieces[white] 					|= (1ULL << field);
					break;
				case 'k':
					position.pieceTables[black][king] 		|= (1ULL << field);
					position.pieces[black] 					|= (1ULL << field);
					break;
				case 'q':
					position.pieceTables[black][queen]  	|= (1ULL << field);
					position.pieces[black] 					|= (1ULL << field);
					break;
				case 'r':
					position.pieceTables[black][rook]  		|= (1ULL << field);
					position.pieces[black] 					|= (1ULL << field);
					break;
				case 'b':
					position.pieceTables[black][bishop]  	|= (1ULL << field);
					position.pieces[black] 					|= (1ULL << field);
					break;
				case 'n':
					position.pieceTables[black][knight]  	|= (1ULL << field);
					position.pieces[black] 					|= (1ULL << field);
					break;
				case 'p':
					position.pieceTables[black][pawn]  		|= (1ULL << field);
					position.pieces[black] 					|= (1ULL << field);
					break;
				default:
					std::cout << "invalid fen  position string " << current << std::endl;
					break;

			}

		}
		ind++;
	}

	ind++;
	if(fen.at(ind) == 'w'){
		position.toMove = white;
	} else {
		position.toMove = black;
	}
	ind++; //should be a space now
	position.data.castlingRights = 0;
	while((fen.at(ind) == ' ') && ( (int) (ind+1) < (int) fen.size())) {
		ind++;
	}

	while(fen.at(ind) != ' ') {
		if (fen.at(ind) == 'K') {
			position.data.castlingRights = position.data.castlingRights | 1;
		}
		if (fen.at(ind) == 'Q') {
			position.data.castlingRights = position.data.castlingRights | 2;
		}
		if (fen.at(ind) == 'k') {
			position.data.castlingRights = position.data.castlingRights | 4;
		}
		if (fen.at(ind) == 'q') {
			position.data.castlingRights = position.data.castlingRights | 8;
		}
		ind++;
	}

	while((fen.at(ind) == ' ') && ( (int) (ind+1) < (int) fen.size())) {
		ind++;
	}
	position.data.enPassantFile = 8;
	position.data.fiftyMoveRuleCounter = 0;

	if (fen.at(ind) != '-') {
		position.data.enPassantFile = fen.at(ind)-'a';
		ind++;
	}

	position.totalFigureEval   = calcTotalFigureEvaluation(&position);
	position.figureEval   = calcFigureEvaluation(&position);
	position.pieceTableEval = ((1 << 15) + position.figureEval+ calcPieceTableValue(&position))
							+(((1 << 14) + calcEndGamePieceTableValue(&position)+position.figureEval) << 16);
	position.zobristHash    = calcZobristHash(&position);
	position.pawnHash       = calcPawnHash(&position);
	position.data.hash = position.zobristHash;

	return position;

}

chessPosition stringToChessPosition(std::string strposition) {

	memset(repetitionData, 0, 16384*sizeof(uint16_t));
	//Not performance-critical
	//---------------------------
	chessPosition position;
	zeroInitPosition(&position);
	//
	if (strposition.length() != 69) {
		std::cout << "Invalid chess position string!!" << std::endl;
		return position;
	}

	for (uint8_t ind=0; ind < 64; ind++) {
		char c = strposition.at(ind);
			switch(c){
				case 'K':
					position.pieces[white] 					|= (1ULL << ind);
					position.pieceTables[white][king] 		|= (1ULL << ind);
					break;
				case 'Q':
					position.pieceTables[white][queen] 		|= (1ULL << ind);
					position.pieces[white] 					|= (1ULL << ind);
					break;
				case 'R':
					position.pieceTables[white][rook] 		|= (1ULL << ind);
					position.pieces[white] 					|= (1ULL << ind);
					break;
				case 'B':
					position.pieceTables[white][bishop] 	|= (1ULL << ind);
					position.pieces[white] 					|= (1ULL << ind);
					break;
				case 'N':
					position.pieceTables[white][knight] 	|= (1ULL << ind);
					position.pieces[white] 					|= (1ULL << ind);
					break;
				case 'P':
					position.pieceTables[white][pawn] 		|= (1ULL << ind);
					position.pieces[white] 					|= (1ULL << ind);
					break;
				case 'k':
					position.pieceTables[black][king] 		|= (1ULL << ind);
					position.pieces[black] 					|= (1ULL << ind);
					break;
				case 'q':
					position.pieceTables[black][queen]  	|= (1ULL << ind);
					position.pieces[black] 					|= (1ULL << ind);
					break;
				case 'r':
					position.pieceTables[black][rook]  		|= (1ULL << ind);
					position.pieces[black] 					|= (1ULL << ind);
					break;
				case 'b':
					position.pieceTables[black][bishop]  	|= (1ULL << ind);
					position.pieces[black] 					|= (1ULL << ind);
					break;
				case 'n':
					position.pieceTables[black][knight]  	|= (1ULL << ind);
					position.pieces[black] 					|= (1ULL << ind);
					break;
				case 'p':
					position.pieceTables[black][pawn]  		|= (1ULL << ind);
					position.pieces[black] 					|= (1ULL << ind);
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

	position.data.castlingRights = 0;
	if (strposition.at(65) == 'K') {
		position.data.castlingRights = position.data.castlingRights | 1;
	}
	if (strposition.at(66) == 'Q') {
		position.data.castlingRights = position.data.castlingRights | 2;
	}
	if (strposition.at(67) == 'k') {
		position.data.castlingRights = position.data.castlingRights | 4;
	}
	if (strposition.at(68) == 'q') {
		position.data.castlingRights = position.data.castlingRights | 8;
	}

	position.totalFigureEval   = calcTotalFigureEvaluation(&position);
	position.figureEval   = calcFigureEvaluation(&position);
	position.pieceTableEval = ((1 << 15) + position.figureEval+ calcPieceTableValue(&position))
							+(((1 << 14) + calcEndGamePieceTableValue(&position)+position.figureEval) << 16);
	position.zobristHash    = calcZobristHash(&position);

	position.pawnHash = calcPawnHash(&position);
	position.data.hash = position.zobristHash;
	position.data.enPassantFile = 8;
	position.data.fiftyMoveRuleCounter = 0;

	debug_incremental_calculations(&position);
	return position;
}


std::string moveToString(chessMove move) {

	if(move.type == castlingKingside){
			if(move.sourceField < 32){
				return "e1g1";
			} else {
				return "e8g8";
			}
	}

	if(move.type == castlingQueenside){
			if(move.sourceField < 32){
				return "e1c1";
			} else {
				return "e8c8";
			}
	}

	uint16_t sourceField = move.sourceField;
	uint16_t targetField = move.targetField;

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

	if(move.type == promotionBishop){
		retString.push_back('b');
	}
	if(move.type == promotionKnight){
		retString.push_back('n');
	}
	if(move.type == promotionRook){
		retString.push_back('r');
	}
	if(move.type == promotionQueen){
		retString.push_back('q');
	}
	return retString;

}

std::string moveToExtendedString(chessMove move){
	std::string ret = moveToString(move);
	ret = ret+" "+std::to_string(((int) move.type))+" "+std::to_string(((int) move.captureType));
	return ret;
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


std::string chessPositionToFenString(const chessPosition position, bool EPD){

	std::string str = chessPositionToString(position);
	std::string FEN = "";

	for(int16_t row=7; row > -1; row--) {
		uint16_t empty = 0;

		for(uint16_t column=0; column < 8; column++){
			uint8_t field = 8*row+column;
			char current = str.at(field);
			if(current == '0'){
				empty++;
			} else {
				if(empty > 0){
					FEN = FEN+std::to_string(empty);
					empty = 0;
				}
				FEN.push_back(current);
			}


		}
		if(empty > 0){
			FEN = FEN+std::to_string(empty);
		}
		if(row > 0) {
			FEN.push_back('/');
		}
	}


	FEN.push_back(' ');

	FEN.push_back(str.at(64));

	FEN.push_back(' ');


	uint8_t castling = position.data.castlingRights;

	if(castling & 1){
		FEN.push_back('K');
	}
	if(castling & 2){
		FEN.push_back('Q');
	}
	if(castling & 4){
		FEN.push_back('k');
	}
	if(castling & 8){
		FEN.push_back('q');
	}
	if(!castling){
		FEN.push_back('-');
	}

	FEN.push_back(' ');

	if(position.data.enPassantFile > 7){
		FEN.push_back('-');
	} else {
		if(position.toMove == white) { //BLACK made the ep move
			uint16_t field = 40+position.data.enPassantFile;
			if(EPD){
				char file = (field & 7)+'a';
				char row  = (field/8)+'0';
				FEN.push_back(file);
				FEN.push_back(row);

			} else {
				FEN = FEN+std::to_string(field);
			}

		} else { //WHITE made the ep move
			uint16_t field = 16+position.data.enPassantFile;
			if(EPD){
				char file = (field & 7)+'a';
				char row  = (field/8)+'0';
				FEN.push_back(file);
				FEN.push_back(row);

			} else {
				FEN = FEN+std::to_string(field);
			}
		}

	}

	if(!EPD){
		FEN.push_back(' ');
		FEN.push_back('0');
		FEN.push_back(' ');

		uint16_t moves = position.madeMoves.length;
		FEN = FEN+std::to_string(moves+1);
	}
	return FEN;

}

bool checkMove(chessPosition& position, std::string move, chessMove* out) {
	vdt_vector<chessMove> moves = vdt_vector<chessMove>(100);
			uint64_t mv = stringToMove(move);
			generateAllMoves(&moves, &position);
			calculateStandardSortEvals(&position, &moves, 0, 0, 0, NO_REFUTATION);
			std::sort(moves.data, moves.data+moves.length);
			bool found = false;
			chessMove m;
			for(uint16_t ind=0; ind < moves.length; ind++) {
				if(moves[ind].sortEval < -10000) {
					break;
				}

				uint64_t current = BIT64(moves[ind].sourceField) | BIT64(moves[ind].targetField);
				if(current == mv){
					if( (moves[ind].type > 8)) { //its a promotion
						char type = move.at(4);
						if((type == 'r') && moves[ind].type != promotionRook){
							continue;
						}
						if((type == 'q') && moves[ind].type != promotionQueen){
							continue;
						}
						if((type == 'b') && moves[ind].type != promotionBishop){
							continue;
						}
						if((type == 'n') && moves[ind].type != promotionKnight){
							continue;
						}
					}
					found = true;
					m = moves[ind];
					break;
				}
				if(moves[ind].type == castlingKingside){
					if(position.toMove == white){
						if(move == "e1g1"){
							found = true;
							m = moves[ind];
							break;
						}
					}
					if(position.toMove == black){
						if(move == "e8g8"){
							found = true;
							m = moves[ind];
							break;
						}
					}
				}
				if(moves[ind].type == castlingQueenside){
					if(position.toMove == white){
						if(move == "e1c1"){
							found = true;
							m = moves[ind];
							break;
						}
					}
					if(position.toMove == black){
						if(move == "e8c8"){
							found = true;
							m = moves[ind];
							break;
						}
					}
				}
			}
			moves.free_array();
			*out = m;
			return found;

}


bool checkAndMakeMove(chessPosition& position, std::string move){

	chessMove m;
	bool found = checkMove(position, move, &m);

		if(found){
			std::cout << moveToString(m) << std::endl;
			makeMove(&m, &position);
			uint16_t kingField = findLSB(position.pieceTables[1-position.toMove][king]);
			if(isFieldAttacked(&position, position.toMove, kingField)){
				//std::cout << "Illegal move!" << std::endl;
				undoMove(&position);
			} else {
				std::string newPosition = chessPositionToString(position);
				//userInterface->sendNewPosition(newPosition);
			}
		} else {
			std::cout << "Invalid move!" << std::endl;
		}

		return found;
}
extern std::atomic<bool> continueSearch;

void runPerformanceTests(){
	continueSearch = true;
	for(int depth = 3; depth < 11; depth++){
		std::ifstream file;
		file.open("chesspositionsfixed.txt");
		std::string line;
		uint64_t negamaxNodes = 0;
		uint64_t qNodes = 0;

		uint32_t nodes = 0;
		while(std::getline(file, line)){
			//std::cout << line << std::endl;
			uint64_t nmNodes = 0;
			uint64_t qn = 0;
			uint64_t newNodes = runSinglePositionPerformanceTest(line, depth, &nmNodes, &qn, true);
			if(newNodes != qn+nmNodes){
				std::cout << "WTF???" << std::endl;
			}
			nodes = nodes + newNodes;
			//std::cout << newNodes << std::endl;
			//std::cout << nmNodes << std::endl;
			//std::cout << qn << std::endl;
			negamaxNodes = negamaxNodes+nmNodes;
			qNodes = qNodes+qn;

		}
		std::cout << "Depth " << depth  << " Nodes " << nodes << std::endl;
		std::cout << "negamaxnodes " <<  negamaxNodes << std::endl;
		std::cout << "qnodes " <<  qNodes << std::endl;
		file.close();

	}
}

void outputUint64(uint64_t num) {
	for(int16_t row = 7; row > -1; row--) {
		for(uint16_t file = 0; file < 8; file++) {
			uint16_t field = 8*row+file;
			if(BIT64(field) & num) {
				std::cout << "1";
			} else {
				std::cout << "0";
			}
		}
		std::cout << std::endl;
	}
}
