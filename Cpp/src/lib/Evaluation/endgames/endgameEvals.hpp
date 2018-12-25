/*
 * endgameEvals.hpp
 *
 *  Created on: Aug 27, 2017
 *      Author: vabi
 */

#ifndef LIB_EVALUATION_ENDGAMES_ENDGAMEEVALS_HPP_
#define LIB_EVALUATION_ENDGAMES_ENDGAMEEVALS_HPP_
#include <lib/Evaluation/evaluation.hpp>

int16_t KRK_endgame(const chessPosition* position);
int16_t KBBK_endgame(const chessPosition* position);
int16_t KBPK_endgame(const chessPosition* position);
int16_t KPK_endgame(const chessPosition* position);
int16_t KBNK_endgame(const chessPosition* position);

EvalComponentResult kingEndgamePosition(const chessPosition* position,
        const evalParameters* par,
        EvalMemory* evalMemory);

#endif /* LIB_EVALUATION_ENDGAMES_ENDGAMEEVALS_HPP_ */
