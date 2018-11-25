/*
 * fenhelper.hpp
 *
 *  Created on: Nov 25, 2018
 *      Author: vabi
 */

#ifndef UTIL_FEN_FENHELPER_HPP_
#define UTIL_FEN_FENHELPER_HPP_

#include <lib/basics.hpp>
#include <string>

chessPosition FENtoChessPosition(std::string fen);
std::string chessPositionToFenString(const chessPosition position, bool EPD = false);

#endif /* UTIL_FEN_FENHELPER_HPP_ */
