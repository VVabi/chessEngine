/*
 * logger.hpp
 *
 *  Created on: Dec 2, 2016
 *      Author: vabi
 */

#ifndef LOGGING_LOGGER_HPP_
#define LOGGING_LOGGER_HPP_

#include <iostream>
#include <fstream>
#include <lib/basics.hpp>
#include <lib/Evaluation/evaluation.hpp>
#include <string>

void latexOutput(const chessPosition* pos, evaluationResult ev, int16_t eval);
void logSearch(uint64_t nodes, uint32_t time, int32_t eval, uint32_t depth, std::string bestMove);
#endif /* LOGGING_LOGGER_HPP_ */
