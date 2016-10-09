/*
 * debugFunctions.hpp
 *
 *  Created on: Sep 19, 2016
 *      Author: vabi
 */

#ifndef LIB_DEBUGFUNCTIONS_DEBUGFUNCTIONS_HPP_
#define LIB_DEBUGFUNCTIONS_DEBUGFUNCTIONS_HPP_

int16_t calcFigureEvaluation(const chessPosition* position);
uint16_t calcTotalFigureEvaluation(const chessPosition* position);
int16_t calcPieceTableValue(const chessPosition* position);
int16_t calcEndGamePieceTableValue(const chessPosition* position);
#endif /* LIB_DEBUGFUNCTIONS_DEBUGFUNCTIONS_HPP_ */
