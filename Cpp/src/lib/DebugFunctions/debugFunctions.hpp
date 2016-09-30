/*
 * debugFunctions.hpp
 *
 *  Created on: Sep 19, 2016
 *      Author: vabi
 */

#ifndef LIB_DEBUGFUNCTIONS_DEBUGFUNCTIONS_HPP_
#define LIB_DEBUGFUNCTIONS_DEBUGFUNCTIONS_HPP_

int16_t calcFigureEvaluation(chessPosition* position);
uint16_t calcTotalFigureEvaluation(chessPosition* position);
int16_t calcPieceTableValue(chessPosition* position);
int16_t calcEndGamePieceTableValue(chessPosition* position);
#endif /* LIB_DEBUGFUNCTIONS_DEBUGFUNCTIONS_HPP_ */
