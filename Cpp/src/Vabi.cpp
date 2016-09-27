//============================================================================
// Name        : Vabi.cpp
// Author      : Fabian Lenhardt
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;
#include <lib/basics.hpp>
#include <lib/moveMaking/moveMaking.hpp>
#include <tests/tests.hpp>
#include <DataTypes/vdt_vector.hpp>
#include <lib/moveGeneration/moveGeneration.hpp>
#include <magicNumberGeneration/magicNumberGeneration.hpp>
#include <fstream>
#include <DataTypes/threadSafeQueue.hpp>
#include <tr1/memory>
#include <communication/Network/message.h>
#include <communication/gen/VMPmessages.h>
#include <communication/Network/network.h>
#include <lib/Attacks/attacks.hpp>
#include <lib/bitfiddling.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <Search/search.hpp>
#include <hashTables/hashTables.hpp>
#include <selfPlaying/selfPlaying.hpp>
#include <lib/Evaluation/evaluation.hpp>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */


extern int32_t completePieceTables[7][2][64];
extern int16_t endGamepieceTables[7][2][64];
extern int16_t pieceTables[7][2][64];
extern uint16_t figureValues[];


uint64_t get_timestamp(){
	struct timeval start;
	uint64_t mtime, seconds, useconds;
	gettimeofday(&start, NULL);
	seconds  = start.tv_sec;
	useconds = start.tv_usec;

	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
	return mtime;

}

void outPutuint64(uint64_t num){

	for	(int j=7; j >= 0; j--)	{
		for(int i=0; i < 8; i++){
			uint64_t toCheck = (1UL << (8*j+i));
			if(num & toCheck) {
				std::cout << "1";
			}
			else{
				std::cout << "0";
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;


}




uint32_t runSinglePositionPerformanceTest(std::string position, uint16_t depth) {
	chessPosition c = stringToChessPosition(position);
	c.castlingRights = 0;
	c.toMove = (position.at(66) == 'w' ? white: black);
	resetNodes();
	chessMove bestMove;
	resetQuiescenceNodes();
	struct timeval start, end;
	//long mtime, seconds, useconds;
	gettimeofday(&start, NULL);
	//int32_t eval = negamax(&c, depth, -100000, 100000, &bestMove);
	negamax(&c, depth, -100000, 100000, &bestMove);
	gettimeofday(&end, NULL);

	/*seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;

	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;*/
	uint32_t nodeCount = getNodes()+getQuiescenceNodes();
	//double nps = ((double) nodeCount)/((double) mtime)*1000.0;
	/*std::cout << "Searched " <<  nodeCount << " positions in " << mtime << " for " << nps << " nodes per second" << std::endl;
	std::cout << "Forced move " << moveToString(bestMove, c) << std::endl;
	std::cout << "Evaluation "  << eval << std::endl;*/

	return nodeCount;

}


int main() {

	for(uint32_t index=0; index < 7; index++) {
		for(uint32_t t=0; t < 2; t++) {
			for(uint32_t k=0; k < 64; k++) {
					completePieceTables[index][t][k] = ((uint16_t) (pieceTables[index][t][k]+figureValues[index])) |  ( ((uint16_t) (endGamepieceTables[index][t][k]+figureValues[index])) << 16);
			}
		}
	}

	srand (time(NULL));
	fillZobristHash();
	/*std::string position = "R000K00RP0000000P00000000000000000000000000000000000ppppr000k00rwKQkq";
	chessPosition c = stringToChessPosition(position);
	std::cout << evaluation(&c) << std::endl;*/
	/*uint64_t pawns = BIT64(10) | BIT64(18) | BIT64(11) | BIT64(12) | BIT64(36);

	std::cout << staticPawnEval(pawns, white) << std::endl;*/

	/*selfPlayResult k = playSelf(3,3,10);
	std::cout << k.engine1Wins<< std::endl;
	std::cout << k.draws << std::endl;
	std::cout << k.engine2Wins << std::endl;*/
	//runTests();
	/*std::string position = "RNBQKBNRPPPPPPPP0q000000000000000000000000000000pppppppprnbqkbnr";
	chessPosition c = stringToChessPosition(position);
	AttackTable t = makeAttackTable(&c, white);

	outPutuint64(t.attackTables[pawn]);
	outPutuint64(t.attackTables[knight]);
	outPutuint64(t.attackTables[bishop]);
	outPutuint64(t.attackTables[rook]);
	outPutuint64(t.attackTables[queen]);*/


	/*for(int depth = 3; depth < 8; depth++) {
		std::ifstream infile("chesspositionsfixed.txt");
		std::string line;
		uint64_t totalNodes = 0;
		while (std::getline(infile, line))
		{
			 totalNodes = totalNodes+runSinglePositionPerformanceTest(line, depth);

		}

		std::cout << "Depth " << depth << " Total Nodes " << totalNodes << std::endl;

	}*/


 	/*std::string position = "RNBQKBNRPPPPPPPP0pr00000000000000000000000000000pppppppprnbqkbnr";
	chessPosition c = stringToChessPosition(position);
	vdt_vector<chessMove> moves = vdt_vector<chessMove>(100);

	generateAllMoves(&moves, &c);
	orderStandardMoves(&c, &moves);

	for (uint16_t ind=0; ind < moves.length; ind++) {
		std::cout << moveToString(moves[ind], c) << " " << moves[ind].sortEval << std::endl;
	}*/


	/*for(uint16_t ind=0; ind < 10; ind++){
		moveVectors[ind] = vdt_vector<chessMove>(buffer+100*ind, 100);
	}
	//std::string position = "R000K00RPPPBBPPP00N00Q0p0p00P000000PN000bn00pnp0p0ppqpb0r000k00r";
	std::string position = "RNBQKBNRPPPPPPPP00000000000000000000000000000000pppppppprnbqkbnr";
	chessPosition c = stringToChessPosition(position);

    struct timeval start, end;

	long mtime, seconds, useconds;

	gettimeofday(&start, NULL);
	uint32_t val = perftNodes(&c, 5);
	gettimeofday(&end, NULL);

	seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;

	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

	std::cout << val  << " in " << mtime << " with " << captures << " captures and " << mates << " mates " << std::endl;




	return 0;*/

	//perftTest();
	initialize_network("127.0.0.1", 9876);
	std::string position = "RNBQKBNRPPPPPPPP00000000000000000000000000000000pppppppprnbqkbnrwKQkq";
	chessPosition c = stringToChessPosition(position);


	while(1){

		receive_from_network();
		auto checkMove = VMP_receive<VMPcheckMove>();
		if(checkMove){
			std::string nextMove = std::string((char*) checkMove->moveString.chars.data, 4);
			vdt_vector<chessMove> moves = vdt_vector<chessMove>(100);
			uint64_t mv = stringToMove(nextMove);
			generateAllMoves(&moves, &c);

			bool found = false;
			chessMove m;
			for(uint16_t ind=0; ind < moves.length; ind++) {
				if(moves[ind].move == mv){
					found = true;
					m = moves[ind];
				}
			}

			if(found){
				std::cout << moveToString(m, c) << std::endl;
				makeMove(&m, &c);

				uint16_t kingField = findLSB(c.pieceTables[1-c.toMove][king]);

				if(isFieldAttacked(&c, c.toMove, kingField)){
					std::cout << "Illegal move!" << std::endl;
					undoMove(&c);
				} else {
					std::string newPosition = chessPositionToString(c);
					fsarray<uint8_t> raw_str = fsarray<uint8_t>(newPosition.length());
					memcpy(raw_str.data, newPosition.c_str(), 64);
					VDTstring str = VDTstring(raw_str);
					auto newPosMsg = std::unique_ptr<VMPchessPosition>(new VMPchessPosition(str));
					send_msg(std::move(newPosMsg), 0);
					chessMove bestMove;
					resetNodes();
					resetQuiescenceNodes();
					resetIndices();

					uint64_t start_ts  = get_timestamp();
					uint16_t depth = 6;
					int32_t eval = 0;
					while(get_timestamp()-start_ts < 2000) {
						eval = negamax(&c, depth, -100005, 100005, &bestMove);
						depth++;
					}
					std::cout << "Depth searched " << depth << std::endl;
					uint64_t mtime = get_timestamp()-start_ts;
					uint32_t nodeCount = getNodes()+getQuiescenceNodes();
					double nps = ((double) nodeCount)/((double) mtime)*1000.0;
					std::cout << "Searched " <<  nodeCount << " positions in " << mtime << " for " << nps << " nodes per second" << std::endl;
					std::cout << "Forced move " << moveToString(bestMove, c) << std::endl;
					std::cout << "Evaluation "  << eval << std::endl;
					int32_t* indices = getIndexCounts();
					for(uint16_t k=0; k < 50; k++){
						std::cout << indices[k] << " ";
					}

					std::cout << std::endl;

	 				makeMove(&bestMove, &c);
					std::string newPosition2 = chessPositionToString(c);
					fsarray<uint8_t> raw_str2 = fsarray<uint8_t>(newPosition2.length());
					memcpy(raw_str2.data, newPosition2.c_str(), 69);
					VDTstring str2 = VDTstring(raw_str2);
					auto newPosMsg2 = std::unique_ptr<VMPchessPosition>(new VMPchessPosition(str2));
					send_msg(std::move(newPosMsg2), 0);
				}
			} else {
				std::cout << "Invalid move!" << std::endl;
			}
			moves.free_array();
			std::cout << "Figureeval " << c.figureEval << std::endl;
			std::cout << "Piece table eval " << c.pieceTableEval << std::endl;
		}

		auto undo = VMP_receive<VMPundoMove>();

		if (undo) {
			std::cout << "Got undo event!" << std::endl;
			undoMove(&c);
			std::string newPosition = chessPositionToString(c);
			fsarray<uint8_t> raw_str = fsarray<uint8_t>(newPosition.length());
			memcpy(raw_str.data, newPosition.c_str(), 69);
			VDTstring str = VDTstring(raw_str);
			auto newPosMsg = std::unique_ptr<VMPchessPosition>(new VMPchessPosition(str));
			send_msg(std::move(newPosMsg), 0);
		}

		auto forceMove =  VMP_receive<VMPforceMove>();

		if(forceMove) {

			chessMove bestMove;
			negamax(&c, 8, -100000, 100000, &bestMove);
			std::cout << "Forced move " << moveToString(bestMove, c) << std::endl;
			makeMove(&bestMove, &c);
			std::string newPosition = chessPositionToString(c);
			fsarray<uint8_t> raw_str = fsarray<uint8_t>(newPosition.length());
			memcpy(raw_str.data, newPosition.c_str(), 69);
			VDTstring str = VDTstring(raw_str);
			auto newPosMsg = std::unique_ptr<VMPchessPosition>(new VMPchessPosition(str));
			send_msg(std::move(newPosMsg), 0);
		}
	}


	/*ofstream o;
	o.open("/home/vabi/rookMoveTables.txt");

	o << generateRookMoveTablesString();
	o.close();*/






	/*std::string position = "RNBQKBNRPPPPPPPP00000000000000000000000000000000pppppppprnbqkbnr";
	chessPosition c = stringToChessPosition(position);
	while(1){

		std::string output = chessPositionToOutputString(c);
		std::cout << output << std::endl;
		vdt_vector<chessMove> moves = vdt_vector<chessMove>(100);

		std::string nextMove;
		cin >> nextMove;
		uint64_t mv = stringToMove(nextMove);
		generateAllMoves(&moves, &c);

		bool found = false;
		chessMove m;
		for(uint16_t ind=0; ind < moves.length; ind++) {
			if(moves[ind].move == mv){
				found = true;
				m = moves[ind];
			}


		}


		if(found){
			std::cout << moveToString(m, c) << std::endl;
			makeMove(&m, &c);

		} else {
			std::cout << "Invalid move!" << std::endl;
		}
		moves.free_array();
	}*/





}
