/*
 * UIlayer.hpp
 *
 *  Created on: Oct 9, 2016
 *      Author: vabi
 */

#ifndef USERINTERFACE_UILAYER_HPP_
#define USERINTERFACE_UILAYER_HPP_
#include <lib/basics.hpp>
#include <string>
#include <userInterface/interfaceStructs.hpp>
#include <lib/Evaluation/evaluation.hpp>
bool doContinueSearch();
void UIloop();
uint32_t searchMove(chessPosition* position, chessMove* bestMove, uint32_t* nodeCount, uint64_t* mtime, int32_t* eval, bool doAspiration, searchParameters params);
std::string chessPositionToString(const chessPosition position);
std::string chessPositionToOutputString(const chessPosition position);
chessPosition stringToChessPosition(std::string strposition);
chessPosition FENtoChessPosition(std::string fen);
std::string chessPositionToFenString(chessPosition position, bool EPD = false);
std::string moveToString(chessMove move);
uint64_t stringToMove(std::string mv);
bool checkAndMakeMove(chessPosition* position, std::string move);
bool checkMove(chessPosition* position, std::string move, chessMove* out);
uint64_t get_timestamp();
void outputUint64(uint64_t num);
uint64_t runSinglePositionPerformanceTest(std::string position, uint16_t depth, uint64_t* negamaxNodes, uint64_t* qNodes, bool useAspiration);
void runPerformanceTests(uint32_t depth);
std::string moveToExtendedString(chessMove move);
void sendSearchInfo(uint64_t nodes, uint32_t time, int32_t eval, uint32_t depth, const std::list<std::string>& PV);
#endif /* USERINTERFACE_UILAYER_HPP_ */
