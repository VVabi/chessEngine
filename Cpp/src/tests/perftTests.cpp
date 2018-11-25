/*
 * perftTests.cpp
 *
 *  Created on: Sep 24, 2016
 *      Author: vabi
 */

#include <string>
#include <lib/basics.hpp>
#include <lib/moveMaking/moveMaking.hpp>
#include <iostream>
#include <tests/tests.hpp>
#include <DataTypes/vdt_vector.hpp>
#include <lib/moveGeneration/moveGeneration.hpp>
#include <lib/Attacks/attacks.hpp>
#include <lib/bitfiddling.h>
#include <Search/search.hpp>
#include <userInterface/UIlayer.hpp>
#include <algorithm>
#include <lib/Defines/figureValues.hpp>

enum sortState {
    not_sorted,
    hash_handled,
    good_captures_handled,
    killers_handled,
    fully_sorted
};
static int16_t figureValues[7] = { PAWNVALUE, KNIGHTVALUE, BISHOPVALUE,
        ROOKVALUE, QUEENVALUE, 10000, 0 };

static inline bool getGoodCaptureToFront(vdt_vector<chessMove>* moves,
        uint16_t start_index) {
    int16_t best_index = -1;

    int16_t best = 0;

    for (uint16_t ind = start_index; ind < moves->length; ind++) {
        chessMove mv = (*moves)[ind];
        if (mv.captureType == none) {
            continue;
        }

        if (((uint16_t) mv.captureType) >= ((uint16_t) mv.type)) { // || ((mv.captureType == knight) && (mv.type == bishopMove))) {
            int16_t local_best = 5 * figureValues[mv.captureType] / 2
                    - figureValues[mv.type];
            if (local_best > best) {
                best = local_best;
                best_index = ind;
            }
        }
    }

    if (best_index > -1) {
        chessMove buffer = (*moves)[start_index];
        (*moves)[start_index] = (*moves)[best_index];
        (*moves)[best_index] = buffer;
        return true;
    }

    return false;
}

static inline bool getHashMoveToFront(vdt_vector<chessMove>* moves,
        uint16_t hashMove, uint16_t startIndex) {
    for (uint16_t ind = startIndex; ind < moves->length; ind++) {
        chessMove mv = (*moves)[ind];
        if ((((mv.sourceField) | (mv.targetField << 8)) == hashMove)
                && (mv.type != castlingKingside)
                && (mv.type != castlingQueenside)) {
            chessMove buffer = (*moves)[startIndex];
            (*moves)[startIndex] = (*moves)[ind];
            (*moves)[ind] = buffer;
            return true;
        }
    }
    return false;
}

uint64_t perftNodes(chessPosition* position, uint16_t depth) {
    if (depth == 0) {
        return 1;
    }
    uint64_t nodes = 0;
    vdt_vector<chessMove> moves = vdt_vector<chessMove>(150);
    generateAllMoves(&moves, position);
    /*calculateStandardSortEvals(position, &moves, 0, 0, 0, NO_REFUTATION);
     std::sort(moves.data, moves.data+moves.length);*/

    sortState currentState = not_sorted;
    uint64_t ownKing = position->pieceTables[position->toMove][king];
    bool movingSideInCheck = isFieldAttacked(position,
            (playerColor) (1 - position->toMove), findLSB(ownKing));
    if (movingSideInCheck) {
        currentState = killers_handled;
    }

    for (uint16_t ind = 0; ind < moves.length; ind++) {
        bool finished = false;

        while (!finished) {
            switch (currentState) {
            case not_sorted:
                assert(ind == 0);
                currentState = hash_handled;
                break;
            case hash_handled:
                if (getGoodCaptureToFront(&moves, ind)) {
                    moves[ind].sortEval = 10000;
                    finished = true;
                    break;
                }
                currentState = good_captures_handled;
                break;
            case good_captures_handled: {
                currentState = killers_handled;
                break;
            }
            case killers_handled:
                //first move didn't produce cutoff, now we need to sort
                //------------------------------------------------------
                calculateStandardSortEvals(position, &moves, ind, 0,
                        sortInfo(false, NO_REFUTATION, 0));

                std::stable_sort(moves.data + ind, moves.data + moves.length);//stable sort makes the engine 100% predictable and comparable between different optimization levels
                currentState = fully_sorted;
                finished = true;
                break;
            case fully_sorted:
                finished = true;
                break;
            }
        }

        if (moves[ind].sortEval < -10000) {
            break;
        }

        if (moves[ind].captureType != none) {
            if (moves[ind].captureType == king) {
                std::cout << "Taking the king should be impossible"
                        << std::endl;
                uint16_t kingField = findLSB(
                        position->pieceTables[1 - position->toMove][king]);
                isFieldAttacked(position, position->toMove, kingField);
            }
        }

        makeMove(&moves[ind], position);
        uint16_t kingField = findLSB(
                position->pieceTables[1 - position->toMove][king]);
        uint32_t additional_nodes = 0;

        if (isFieldAttacked(position, position->toMove, kingField)) {
        } else {
            additional_nodes = perftNodes(position, depth - 1);

            nodes = nodes + additional_nodes;
        }

        undoMove(position);
    }

    moves.free_array();
    return nodes;
}
