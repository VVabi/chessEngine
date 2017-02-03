/*
 * parameters.cpp
 *
 *  Created on: Feb 2, 2017
 *      Author: vabi
 */



#include "externalParamReader.hpp"
#include <iostream>
#include "parameters.hpp"
#include <lib/basics.hpp>
#include <lib/Defines/figureValues.hpp>
#include <lib/Defines/evalParameters.hpp>

evalParameters evaluationParameters;

void paramDefaultInit(preParameters* par) {
	par->pawnValue 		= PAWNVALUE;
	par->knightValue 	= KNIGHTVALUE;
	par->bishopValue 	= BISHOPVALUE;
	par->rookValue  	= ROOKVALUE;
	par->queenValue 	= QUEENVALUE;
	par->bishoppair     = BISHOPPAIR;
	par->rookonopenfile = ROOKONOPENFILE;
	par->isolatedpawns  = ISOLATEDPAWN;
	par->isolateddoublepawns = ISOLATEDDOUBLEPAWN;
	par->nonisolateddoublepawns = NONISOLATEDDOUBLEPAWN;
}

staticPawnEvalParameters  initializeStaticPawnParameters(const preParameters par) {
	staticPawnEvalParameters ret_par;
	ret_par.isolatedDoublePawn 		= par.isolateddoublepawns;
	ret_par.isolatedPawn      		= par.isolatedpawns;
	ret_par.nonIsolatedDoublePawn 	= par.nonisolateddoublepawns;
	return ret_par;
}


void initializeDependentParameters(preParameters par) {
	evaluationParameters.figureValues[pawn] 		= par.pawnValue;
	evaluationParameters.figureValues[knight] 		= par.knightValue;
	evaluationParameters.figureValues[bishop] 		= par.bishopValue;
	evaluationParameters.figureValues[rook] 		= par.rookValue;
	evaluationParameters.figureValues[queen] 		= par.queenValue;
	evaluationParameters.figureValues[king] 		= 10000;
	evaluationParameters.figureValues[none]			= 0;
	evaluationParameters.bishoppair                 = par.bishoppair;
	evaluationParameters.rookOnOpenFile				= par.rookonopenfile;
	evaluationParameters.staticPawnParameters       = initializeStaticPawnParameters(par);
}

void initializeParameters() {
	parameterReader reader = parameterReader();
	preParameters par;
	paramDefaultInit(&par);
	reader.readParameters(&par, "params.txt");
	std::cout << par.pawnValue << std::endl;
	std::cout << par.knightValue << std::endl;
	std::cout << par.bishopValue << std::endl;
	std::cout << par.rookValue << std::endl;
	std::cout << par.queenValue << std::endl;
	initializeDependentParameters(par);
}
