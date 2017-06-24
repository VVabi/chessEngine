/*
 * parameters.hpp
 *
 *  Created on: Feb 2, 2017
 *      Author: vabi
 */

#ifndef PARAMETERS_PARAMETERS_HPP_
#define PARAMETERS_PARAMETERS_HPP_
#include "parametersPrivate.hpp"

struct staticPawnEvalParameters{
	int16_t isolatedPawn;
	int16_t isolatedDoublePawn;
	int16_t nonIsolatedDoublePawn;
};

struct kingSafetyEvalParameters{
	int16_t selfopenfiletoking;
	int16_t selfopenfilenexttoking;
	int16_t opponentopenfiletoking;
	int16_t opponentopenfilenexttoking;
};

struct evalParameters{
	int16_t figureValues[7];
	int16_t bishoppair;
	int16_t rookOnOpenFile;
	staticPawnEvalParameters staticPawnParameters;
	kingSafetyEvalParameters kingSafetyParameters;
};

extern evalParameters evaluationParameters;
inline const evalParameters* getEvalParameters() {
	return &evaluationParameters;
}

void initializeParameters();
preParameters* getPreParameters();
void initializeDependentParameters(preParameters par);
#endif /* PARAMETERS_PARAMETERS_HPP_ */
