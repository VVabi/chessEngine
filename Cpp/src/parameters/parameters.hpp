/*
 * parameters.hpp
 *
 *  Created on: Feb 2, 2017
 *      Author: vabi
 */

#ifndef PARAMETERS_PARAMETERS_HPP_
#define PARAMETERS_PARAMETERS_HPP_


struct staticPawnEvalParameters{
	int16_t isolatedPawn;
	int16_t isolatedDoublePawn;
	int16_t nonIsolatedDoublePawn;
};

struct evalParameters{
	int16_t figureValues[7];
	int16_t bishoppair;
	int16_t rookOnOpenFile;
	staticPawnEvalParameters staticPawnParameters;
};

extern evalParameters evaluationParameters;
inline const evalParameters* getEvalParameters() {
	return &evaluationParameters;
}

void initializeParameters();

#endif /* PARAMETERS_PARAMETERS_HPP_ */
