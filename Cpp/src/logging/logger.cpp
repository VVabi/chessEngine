/*
 * logger.cpp
 *
 *  Created on: Dec 2, 2016
 *      Author: vabi
 */


#include "logger.hpp"
#include <lib/DebugFunctions/debugFunctions.hpp>
#include <userInterface/UIlayer.hpp>
static int clearPageCounter = 0;

std::ofstream evalDebugging("/home/vabi/TeX/Chess/gamePositions.tex");

void putTableLine(std::ostream& out, int16_t value, std::string name) {
	out << name << " & " << value << "\\\\ \\hline" << std::endl;
}


void latexOutput(const chessPosition* pos, evaluationResult ev, int16_t eval, std::ostream& out) {

	int16_t figureVal = calcFigureEvaluation(pos);
	out << "\\begin{figure}[!htbp] " << std::endl;
	out << "\\newgame" << std::endl;
	out << "\\fenboard{"+chessPositionToFenString(*pos, false)+"}" << std::endl;

	out << "\\begin{tabular}{ll}" << std::endl;
	out << "\\raisebox{-.5\\height}{\\showboard}&   \\begin{tabular}{ l | c  }" << std::endl;

	putTableLine(out, eval, "Total");
	putTableLine(out, figureVal, "Material");
	putTableLine(out, ev.PSQ-figureVal, "Piece Square Tables");
	putTableLine(out, ev.mobility, "Mobility");
	putTableLine(out, ev.kingSafety, "King Safety");
	putTableLine(out, ev.bishoppair, "Bishop pair");
	putTableLine(out, ev.staticPawn, "Pawn structure");
	putTableLine(out, ev.passedPawn, "Passed pawns");
	putTableLine(out, ev.rookOpenFiles, "Rooks on open files");
	out << "\\end{tabular}" << std::endl;
	out << "\\end{tabular}" << std::endl;
	out << "\\end{figure}" << std::endl;
	clearPageCounter++;
	if(clearPageCounter == 3) {
		out << "\\clearpage" << std::endl;
		clearPageCounter = 0;
	}
}

void latexOutput(std::string FEN, std::ostream& out) {
	chessPosition pos = FENtoChessPosition(FEN);
	int16_t eval = evaluation(&pos, -32000, 32000);
	evaluationResult ev = getEvaluationResult();
	latexOutput(&pos, ev, eval, out);
}


void latexOutput(const chessPosition* pos, evaluationResult ev, int16_t eval){
	latexOutput(pos, ev, eval, evalDebugging);
}



