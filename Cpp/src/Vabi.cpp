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


static chessMove buffer[900];
uint32_t captures = 0;
uint32_t mates    = 0;


vdt_vector<chessMove> moveVectors[10];

uint32_t perftNodes(chessPosition* c, uint16_t depth){
	if(depth == 0){
		return 1;
	}
	uint32_t nodes = 0;
	vdt_vector<chessMove> moves = moveVectors[depth];
	generateAllMoves(&moves, c);
	bool isMate = true;
	for(uint16_t ind=0; ind < moves.length; ind++){
		if(moves[ind].captureType != none){
			if(moves[ind].captureType == king){
				std::cout << "Taking the king should be impossible" << std::endl;
				uint16_t kingField = findLSB(c->pieceTables[1-c->toMove][king]);
				isFieldAttacked(c, c->toMove, kingField);
			}
		}
		makeMove(&moves[ind], c);
		uint16_t kingField = findLSB(c->pieceTables[1-c->toMove][king]);
		uint32_t additional_nodes = 0;
		if(isFieldAttacked(c, c->toMove, kingField)){

		} else {
			isMate = false;
			if(moves[ind].captureType != none){
				captures++;
			}
			additional_nodes = perftNodes(c, depth-1);
			nodes = nodes+additional_nodes;


		}
		undoMove(c);
		if(!isMate){
			if(depth == 6) {
				std::cout << moveToString(moves[ind], *c) << " : " << additional_nodes << std::endl;
			}
		}
	}
	if(isMate){
		mates++;
	}
	return nodes;
}


uint32_t runSinglePositionPerformanceTest(std::string position, uint16_t depth) {
	chessPosition c = stringToChessPosition(position);
	c.castlingRights = 0;
	c.toMove = (position.at(66) == 'w' ? white: black);
	resetNodes();
	chessMove bestMove;
	resetQuiescenceNodes();
	struct timeval start, end;
	long mtime, seconds, useconds;
	gettimeofday(&start, NULL);
	int32_t eval = negamax(&c, depth, -100000, 100000, &bestMove);
	gettimeofday(&end, NULL);

	seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;

	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
	uint32_t nodeCount = getNodes()+getQuiescenceNodes();
	double nps = ((double) nodeCount)/((double) mtime)*1000.0;
	std::cout << "Searched " <<  nodeCount << " positions in " << mtime << " for " << nps << " nodes per second" << std::endl;
	std::cout << "Forced move " << moveToString(bestMove, c) << std::endl;
	std::cout << "Evaluation "  << eval << std::endl;

	return nodeCount;

}


int main() {
	/*std::string position = "RNBQKBNRPPPPPPPP0q000000000000000000000000000000pppppppprnbqkbnr";
	chessPosition c = stringToChessPosition(position);
	AttackTable t = makeAttackTable(&c, white);

	outPutuint64(t.attackTables[pawn]);
	outPutuint64(t.attackTables[knight]);
	outPutuint64(t.attackTables[bishop]);
	outPutuint64(t.attackTables[rook]);
	outPutuint64(t.attackTables[queen]);*/

	/*std::ifstream infile("chesspositions.txt");
	std::string line;
	uint32_t totalNodes = 0;
	while (std::getline(infile, line))
	{
		 totalNodes = totalNodes+runSinglePositionPerformanceTest(line, 3);
	    // process pair (a,b)
	}

	std::cout << "Total Nodes " << totalNodes << std::endl;*/

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
	initialize_network("127.0.0.1", 9876);
	std::string position = "RNBQKBNRPPPPPPPP00000000000000000000000000000000pppppppprnbqkbnr";
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
					struct timeval start, end;
					long mtime, seconds, useconds;
					gettimeofday(&start, NULL);
					int32_t eval = negamax(&c, 6, -100005, 100005, &bestMove);
					gettimeofday(&end, NULL);

					seconds  = end.tv_sec  - start.tv_sec;
					useconds = end.tv_usec - start.tv_usec;

					mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
					uint32_t nodeCount = getNodes()+getQuiescenceNodes();
					double nps = ((double) nodeCount)/((double) mtime)*1000.0;
					std::cout << "Searched " <<  nodeCount << " positions in " << mtime << " for " << nps << " nodes per second" << std::endl;
					std::cout << "Forced move " << moveToString(bestMove, c) << std::endl;
					std::cout << "Evaluation "  << eval << std::endl;
	 				makeMove(&bestMove, &c);
					std::string newPosition2 = chessPositionToString(c);
					fsarray<uint8_t> raw_str2 = fsarray<uint8_t>(newPosition2.length());
					memcpy(raw_str2.data, newPosition2.c_str(), 64);
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
			memcpy(raw_str.data, newPosition.c_str(), 64);
			VDTstring str = VDTstring(raw_str);
			auto newPosMsg = std::unique_ptr<VMPchessPosition>(new VMPchessPosition(str));
			send_msg(std::move(newPosMsg), 0);
		}

		auto forceMove =  VMP_receive<VMPforceMove>();

		if(forceMove) {

			chessMove bestMove;
			int32_t eval = negamax(&c, 4, -100000, 100000, &bestMove);
			std::cout << "Forced move " << moveToString(bestMove, c) << std::endl;
			makeMove(&bestMove, &c);
			std::string newPosition = chessPositionToString(c);
			fsarray<uint8_t> raw_str = fsarray<uint8_t>(newPosition.length());
			memcpy(raw_str.data, newPosition.c_str(), 64);
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
