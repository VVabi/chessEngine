/*
 * moveGeneration.hpp
 *
 *  Created on: Sep 4, 2016
 *      Author: vabi
 */

#ifndef LIB_MOVEGENERATION_MOVEGENERATION_HPP_
#define LIB_MOVEGENERATION_MOVEGENERATION_HPP_


void generateAllMoves(vdt_vector<chessMove>* vec, chessPosition* position);
void generateAllCaptureMoves(vdt_vector<chessMove>* vec, chessPosition* position);
void generateChecks(vdt_vector<chessMove>* vec, chessPosition* position);
#endif /* LIB_MOVEGENERATION_MOVEGENERATION_HPP_ */
