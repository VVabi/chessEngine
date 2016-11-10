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
void UIloop();
uint32_t searchMove(chessPosition* position, chessMove* bestMove, uint32_t maximal_time, uint32_t* nodeCount, uint64_t* mtime, int32_t* eval, bool doAspiration = true);
std::string chessPositionToString(chessPosition position);
std::string chessPositionToOutputString(chessPosition position);
chessPosition stringToChessPosition(std::string strposition);
chessPosition FENtoChessPosition(std::string fen);
std::string chessPositionToFenString(chessPosition position, bool EPD=false);
std::string moveToString(chessMove move, chessPosition position);
uint64_t stringToMove(std::string mv);
bool checkAndMakeMove(chessPosition& position, std::string move);
void runPerformanceTests();
uint64_t get_timestamp();
std::string moveToExtendedString(chessMove move, chessPosition position);
#endif /* USERINTERFACE_UILAYER_HPP_ */
