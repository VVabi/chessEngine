/*
 * negamax.cpp
 *
 *  Created on: Sep 20, 2016
 *      Author: vabi
 */

#include <iostream>
#include <lib/basics.hpp>
#include <lib/moveMaking/moveMaking.hpp>
#include <tests/tests.hpp>
#include <DataTypes/vdt_vector.hpp>
#include <lib/moveGeneration/moveGeneration.hpp>
#include <lib/Attacks/attacks.hpp>
#include <lib/bitfiddling.h>
#include "search.hpp"
#include <hashTables/hashTables.hpp>
#include <algorithm>
#include <assert.h>
#include <lib/Evaluation/evaluation.hpp>
#include <fstream>
#include <userInterface/UIlayer.hpp>
#include <atomic>
#include <Search/history.hpp>
#include <lib/Defines/figureValues.hpp>
#include <Search/killerMoves.hpp>
#include <lib/moveGeneration/moveGenerationInternals.hpp>
#include <Search/repetition.hpp>
#include <util/FEN/fenhelper.hpp>

searchDebugData searchCounts;

static int16_t figureValues[7] = {PAWNVALUE, KNIGHTVALUE, BISHOPVALUE, ROOKVALUE, QUEENVALUE, 10000, 0};

searchDebugData getSearchData() {
    return searchCounts;
}

void resetSearchData() {
    memset(&searchCounts, 0, sizeof(searchCounts));
}

static uint32_t totalTime;
static uint64_t start_ts;

void setTotalTime(uint32_t tTime, uint64_t start) { //TODO: move to separate module which handles all timeouts/stops
    totalTime = tTime;
    start_ts = start;
}


enum sortState {not_sorted, hash_handled, good_captures_handled, killers_handled, fully_sorted};

static moveStack qmvStack;

static inline bool getGoodCaptureToFront(vdt_vector<chessMove>* moves, uint16_t start_index) {
    int16_t best_index = -1;
    int16_t best = 0;
    for (uint16_t ind = start_index; ind < moves->length; ind++) {
        chessMove mv = (*moves)[ind];
        if (mv.captureType == none) {
            continue;
        }

        if (((uint16_t) mv.captureType) >= ((uint16_t) mv.type)) { // || ((mv.captureType == knight) && (mv.type == bishopMove))) {
            int16_t local_best = 5*figureValues[mv.captureType]/2-figureValues[mv.type];
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

static inline bool getHashMoveToFront(vdt_vector<chessMove>* moves, uint16_t hashMove, uint16_t startIndex) {
    for (uint16_t ind = startIndex; ind < moves->length; ind++) {
        chessMove mv = (*moves)[ind];
        if ((((mv.sourceField) | (mv.targetField << 8)) == hashMove) && (mv.type != castlingKingside) && (mv.type != castlingQueenside)) {
             chessMove buffer = (*moves)[startIndex];
             (*moves)[startIndex] = (*moves)[ind];
             (*moves)[ind] = buffer;
             return true;
        }
    }
    return false;
}

static inline void get_extensions_reductions(chessPosition* position, uint16_t* reduction, uint16_t* extension, bool check, bool movingSideInCheck, plyInfo plyinfo, int16_t depth, chessMove* move, uint16_t ind, int16_t bestIndex) {
        if ((bestIndex == -1) && !check && !movingSideInCheck && (move->captureType == none) && (depth > 2) && (plyinfo.ply > 0) && (ind > 2)) {
            if (move->sortEval < 50) {
                *reduction = 1;
                //TODO: this is questionably brutal
                    if ((move->sortEval < -50)) {
                        *reduction = 2;
                        if (depth > 10) {
                            *reduction = 3;
                        }
                }
            }
        }
        if (check) {
            *reduction = 0; //TODO: A check should NEVER be reduced, independent of the ply/depth stuff
            if (((plyinfo.ply+depth < plyinfo.max_ply-1) || ((depth == 1) && (plyinfo.ply+depth < plyinfo.max_ply)) )) {
                if (SEE(position, move) > -50) {
                    *extension = 1;
                }
            }
        }
        bool closeToPromotion = (move->type == pawnMove) && ((move->targetField > 48) || (move->targetField < 16));

        if ((closeToPromotion || (move->type == promotionQueen)) && (plyinfo.ply+depth+*extension < plyinfo.max_ply-1)) {
            *extension = *extension+1;
            *reduction = 0;
            if (plyinfo.ply+depth+*extension < plyinfo.max_ply-1 && position->totalFigureEval < 3000 && (depth < 2)) {
                *extension = *extension+1;
            }
        }
}

static inline bool backtrack_position_for_repetition(chessPosition* position) {
    int16_t numMovesToCheck = position->data.fiftyMoveRuleCounter;
    uint64_t current_hash = position->zobristHash;
    assert(static_cast<int>(position->madeMoves.length)-static_cast<int>(numMovesToCheck) >= 0);
    assert(position->madeMoves.length == position->dataStack.length);
    for (int32_t ind = static_cast<int32_t>(position->madeMoves.length-1); ind >= static_cast<int32_t>(position->madeMoves.length)-numMovesToCheck; ind--) {
    	//only check to next nullmove. TODO: disable completely after a nullmove?
    	//---------------------------------
    	if ((position->madeMoves[ind].sourceField | position->madeMoves[ind].targetField) == 0) {
    		break;
    	}


    	if (position->dataStack[ind].hash == current_hash) {
            return true;
        }
    }
    return false;
}

static uint8_t nullmoveReductions[40] = {0, 1, 2, 2, 2, 3, 3, 3,
                                         3, 3, 3, 3, 3, 3, 3, 3,
                                         3, 3, 3, 3, 3, 3, 3, 3,
                                         3, 3, 3, 3, 3, 3, 3, 3,
                                         3, 3, 3, 3, 3, 3, 3, 3,
};

static inline bool check_futility(bool movingSideInCheck, int32_t alpha, chessPosition* position, int16_t premargin, int16_t margin) {
    if (!movingSideInCheck && (alpha > -2000)) {
        searchCounts.futility_tried++;
        int32_t simpleEval = evaluation(position, alpha-premargin-1, alpha, true);
        if (simpleEval < alpha-premargin) {
            int32_t base = evaluation(position, alpha-margin-1, alpha);
            if (base+margin < alpha) {
                searchCounts.futility_successful++;
                //in this case, trying a silent move is pointless.
                //std::cout << "Successful futility pruning" << std::endl;
                return true;
            }
        }
    }
    return false;
}

static inline bool check_nullmove(chessPosition* position, uint16_t* refutationMoveTarget, uint16_t ply, uint16_t max_ply, int16_t depth, int32_t beta, searchSettings settings) {

	if (beta > 10000) { //TODO: more dynamic condition here?
        return false;
    }

//#ifdef EXPERIMENTAL
    bool noPieces = ((position->pieceTables[position->toMove][knight] || position->pieceTables[position->toMove][bishop] ||position->pieceTables[position->toMove][rook] ||position->pieceTables[position->toMove][queen]) == 0);
    bool opponentNoPawns = (position->pieceTables[1-position->toMove][pawn] == 0);


    if (noPieces || opponentNoPawns) {
        return false;
    }
//#endif

    int32_t eval = evaluation(position, beta-1, beta);
    if (eval < beta) {
        return false; //no point in trying nullmove when the current evaluation is already worse than beta
    }

    makeNullMove(position);
    searchCounts.nullMovePruningTried++;
    pvLine dummy;
    dummy.numMoves = 0;
    dummy.line[0].targetField = NO_REFUTATION;
    int32_t value = -negamax(position, plyInfo(ply+1, max_ply-nullmoveReductions[depth], 0, depth-1-nullmoveReductions[depth]), AlphaBeta(-beta, -beta+1), &dummy, searchSettings(nullmove_disabled, hashprobe_enabled, checkextension_enabled, settings.searchId));
    undoNullMove(position);
    if (value < beta) {
        *refutationMoveTarget = dummy.line[0].targetField;
        assert(*refutationMoveTarget < 65);
        return false; //didnt manage a cutoff :(
    }
    searchCounts.nullMovePruningSuccessful++;
    return true;
}

static inline bool checkHashTable(int16_t* eval, uint16_t* hashMove, uint16_t* hashdepth, HashprobeSetting setting, uint64_t zobristHash, int16_t* alpha, int16_t* beta, int16_t depth, int16_t ply, uint8_t searchId) {
    HashEntry hashVal      = getHashTableEntry(zobristHash);
    uint32_t zobristHigher = (uint32_t) (zobristHash >> 32);
    uint16_t zobristLower  = (uint16_t) (((uint32_t) (zobristHash & 0xFFFFFFFF)) >> 16);
    bool isHit = (zobristHigher == hashVal.hashHighBits) && (zobristLower == hashVal.hashLower);

    if (isHit) {
       *hashMove = hashVal.bestMove;
       *hashdepth = hashVal.depth;
    }

    assert(!((*hashMove != 0) && !isHit));
    if (setting == hashprobe_enabled) {
        if (isHit) {
            int16_t oldEval  = hashVal.eval;
            bool isMateScore = (std::abs(oldEval) > 10000) && (std::abs(oldEval) < 30001);



            if ((depth <= hashVal.depth) && (oldEval != 0)) { //TODO: the != 0 is stupid, but somewhere something goes wrong with 3fold rep scores, so excluded here for safety
                if (((hashVal.flag == FAILHIGH) || (hashVal.flag == FULLSEARCH)) && (oldEval >= *beta) && (!isMateScore || oldEval > *beta+1000)) {
                    setSearchId(searchId, zobristHash, hashVal.index);
                    *eval = *beta;
                    return true;
                } else if (((hashVal.flag == FAILLOW) || (hashVal.flag == FULLSEARCH)) && (oldEval <= *alpha) && (!isMateScore || oldEval < *alpha-1000)) {
                    setSearchId(searchId, zobristHash, hashVal.index);
                    *eval =  *alpha; //node will fail low
                    return true;
                } else if ((hashVal.flag == FULLSEARCH)) { //TODO: this condition can be vastly improved
                    setSearchId(searchId, zobristHash, hashVal.index);
                    if (isMateScore) {
                        if (oldEval < 0) {
                            oldEval += ply;
                        } else {
                            oldEval -= ply;
                        }
                    }
                    *eval = oldEval; //guaranteed to be in (alpha, beta)
                    assert(((oldEval > *alpha) && (oldEval < *beta)) || isMateScore);
                    return true;
                } /*else if ((hashVal.flag == FAILHIGH) && (oldEval > *alpha)) {
                    *alpha = oldEval;
                } else if ((hashVal.flag == FAILLOW) && (oldEval < *beta)) {
                    *beta = oldEval;
                }*/ //This could be used to tighten the window a bit, but it's actually pretty tricky to get right I think.
            }
        }
    }
    return false;
}


static inline void updateHistoryTables(chessMove* bestMove, int16_t depth, vdt_vector<chessMove>* moves, uint16_t bestIndex, playerColor toMove) {
    HistoryTables* historyTables = getHistoryTables();
    if (bestMove->captureType == none) {
        historyTables->changeEntry(toMove, bestMove->sourceField, bestMove->targetField, depth*depth);
    }
    for (uint16_t cnt = 0; cnt < bestIndex; cnt++) {
        chessMove mv = (*moves)[cnt];
        if (mv.captureType == none) {
            historyTables->changeEntry(toMove, mv.sourceField, mv.targetField, -depth*depth);
        }
    }
}

static inline void handleBetaCutoff(chessMove* bestMove, uint64_t zobristHash, int16_t beta, int16_t depth, uint16_t ply, uint8_t searchId) {
    setHashEntry(FAILHIGH, beta, depth, searchId, (bestMove->sourceField | (bestMove->targetField << 8)), zobristHash);
    if (bestMove->captureType == none) {
        uint16_t toRemember = (bestMove->sourceField | (bestMove->targetField << 8));
        killerTable* table = getKillerTable();
        table->setKillerMove(ply, toRemember);
    }
}


static inline void checkTimeout() {
    if (get_timestamp()-start_ts >= totalTime) { //TODO: how is this performance wise?
        qmvStack.reset();
        //std::cout << "Total time " << totalTime << std::endl;
        throw timeoutException();
    }
    if (!doContinueSearch()) {
        qmvStack.reset();
        throw timeoutException();
    }
}

//std::ofstream plogger("/home/vabi/Tools/log.txt");

#define DEFAULT_SORTEVAL 10000

static inline bool get_next_move_to_front(chessPosition* position, sortState* currentState, vdt_vector<chessMove> moves, uint16_t ind, plyInfo plyinfo, sortInfo sortinfo) {
    bool sortedNextMove = false;
    switch (*currentState) {
            case not_sorted:
                assert(ind == 0);
                if (getHashMoveToFront(&moves, sortinfo.hashMove, ind)) {
                    moves[ind].sortEval = DEFAULT_SORTEVAL;
                    sortedNextMove = true;
                }
                *currentState = hash_handled;
                break;
            case hash_handled:
                if (getGoodCaptureToFront(&moves, ind)) {
                    moves[ind].sortEval = DEFAULT_SORTEVAL;
                    sortedNextMove = true;
                } else {
                    *currentState = good_captures_handled;
                }
                break;
            case good_captures_handled: {
                killerTable* table = getKillerTable();
                if (getHashMoveToFront(&moves, table->getKillers(plyinfo.ply).killers[0], ind)) {
                    moves[ind].sortEval = DEFAULT_SORTEVAL;
                    sortedNextMove = true;
                } else if (getHashMoveToFront(&moves, table->getKillers(plyinfo.ply).killers[1], ind)) {
                    moves[ind].sortEval = DEFAULT_SORTEVAL;
                    sortedNextMove = true;
                } else {
                    *currentState = killers_handled;
                }
                break;
            }
            case killers_handled:
                //first move didn't produce cutoff, now we need to sort
                //------------------------------------------------------
                calculateStandardSortEvals(position, &moves, ind, plyinfo.ply, sortinfo);
                searchCounts.neededSort++;
                std::stable_sort(moves.data+ind, moves.data+moves.length);//stable sort makes the engine 100% predictable and comparable between different optimization levels
                *currentState = fully_sorted;
                sortedNextMove = true;
                break;
            case fully_sorted:
                sortedNextMove = true;
                break;
        }
    return sortedNextMove;
}

static inline searchLoopResults negamax_internal_move_loop(chessPosition* position, vdt_vector<chessMove> moves, AlphaBeta alphabeta, plyInfo plyinfo, sortInfo sortinfo, pvLine* PV, searchSettings settings) {
    uint16_t numlegalMoves = 0;

    //remembers the index of best move after sorting
    //-------------------------------------------
    int16_t bestIndex = -1;

    pvLine localPV;
    localPV.numMoves = 0;

    sortState currentState = not_sorted;
    if (sortinfo.movingSideInCheck) {
        currentState = killers_handled;
    }

    for (uint16_t ind = 0; ind < moves.length; ind++) {
            while (!get_next_move_to_front(position, &currentState, moves, ind, plyinfo, sortinfo)) {}

            //illegal move. Since list is sorted or, in case ind = 0, best move is first, we can leave here: all further moves are also illegal.
            //---------------------------------------------------------------------------------------------------------------------------------
            if (moves[ind].sortEval < -10000) {
                //TODO: we should add this check, but since we count nodes in makeMove, this destroys the debug vs release tests
/*#ifdef DEBUG
                makeMove(&moves[ind], position);
                uint16_t kingField = findLSB(position->pieceTables[1- position->toMove][king]);
                if (!isFieldAttacked(position,  position->toMove, kingField) && moves[ind].type != castlingKingside && moves[ind].type != castlingQueenside) {
                    std::cout << chessPositionToFenString(*position, false) << std::endl;
                    std::cout << moveToString(moves[ind]) << std::endl;

                    logError("Move wrongly classified as illegal by move ordering");
                }
                undoMove(position);
#endif*/
                break;
            }

            //now make move and check for legality
            //---------------------------------------
            makeMove(&moves[ind], position);

            uint16_t kingField = findLSB(position->pieceTables[1- position->toMove][king]);

            if (sortinfo.movingSideInCheck || (BIT64(moves[ind].sourceField) & (getRookMoves(kingField) | getBishopMoves(kingField))) || (moves[ind].type == kingMove)) {
                if (isFieldAttacked(position,  position->toMove, kingField)) {
                    /*if (moves[ind].type == kingMove) {
                        std::cout << chessPositionToFenString(*position) << std::endl;
                    }*/
                    //move exposed our king, undo and continue
                    //---------------------------------------------
                    //assert(moves[ind].type != kingMove); //all king moves moving into check should be found by move ordering!
                    undoMove(position);
                    continue;
                }
            }
    #ifdef DEBUG
            //debug check: if we overlooked an illegal move, we scream here
            //-------------------------------------------------------------
            if (isFieldAttacked(position,  position->toMove, kingField)) {
                logError("Illegal move detected");
            }
    #endif
            searchCounts.nodes[plyinfo.depth]++;
            searchCounts.totalNodes++;
            numlegalMoves++;

            uint16_t ownkingField = findLSB(position->pieceTables[position->toMove][king]);

            //check whether last move gave check (so its a check-check)
            //----------------------------------------------------------------
            bool check = isFieldAttacked(position,  (playerColor) (1-position->toMove), ownkingField);

            //get extensions and reductions
            //------------------------------
            uint16_t reduction = 0;
            uint16_t extension = 0;
            get_extensions_reductions(position, &reduction, &extension, check, sortinfo.movingSideInCheck, plyinfo, plyinfo.depth, &moves[ind], ind, bestIndex);
            if ((settings.checkextensionSetting == checkextension_disabled)) {
                extension = 0;
            }

            //PVSearch, currently a small gain for us with the > 2
            //-------------------------------------------------
            if (((ind > 2) || (bestIndex != -1)) && (plyinfo.depth > 2)) {
                int32_t value = -negamax(position, plyinfo.increment(extension-reduction), alphabeta.zeroWindow().invert(), &localPV, searchSettings(settings.searchId));
                if (value < alphabeta.alpha+1) {
                    undoMove(position);
                    continue;
                }
            }

            //this is the real, full-fledged search now
            //-------------------------------------------
            int32_t value = -negamax(position, plyinfo.increment(extension), alphabeta.invert(), &localPV, searchSettings(settings.searchId));

            //in case move is better than previous, remember
            //------------------------------------------------
            if (alphabeta.update(value)) {
                PV->line[0] = moves[ind];
                memcpy(PV->line+1, localPV.line, localPV.numMoves*sizeof(chessMove));
                PV->numMoves =  localPV.numMoves+1;
                bestIndex = ind;
            }
            undoMove(position);
            //in case of beta cutoff, leave
            //----------------------------------
            if (alphabeta.betacutoff()) {
                break;
            }
        }
        return searchLoopResults(alphabeta, bestIndex, numlegalMoves);
}

static inline int16_t negamax_internal(chessPosition* position, plyInfo plyinfo, AlphaBeta alphabeta, pvLine* PV, searchSettings settings, sortInfo sortinfo) {
    //generate moves
    //------------------
    uint16_t stackCounter = qmvStack.getCounter();
    vdt_vector<chessMove> moves = qmvStack.getNext();
    generateAllMoves(&moves, position);

    //init variables
    //-----------------------------
    searchCounts.wentToSearch++;

    searchLoopResults res = negamax_internal_move_loop(position, moves, alphabeta, plyinfo, sortinfo, PV, settings);
    if (res.alphabeta.betacutoff()) {
        assert(!res.noMovesAvailable());
        assert(res.foundGoodMove());
        handleBetaCutoff(&PV->line[0], position->zobristHash, alphabeta.beta, plyinfo.depth, plyinfo.ply, settings.searchId);
        if (res.foundGoodMove()) {
                assert((res.bestIndex >= 0)&& (res.bestIndex < ((int32_t) moves.length)));
                if (PV->line[0].captureType == none) {
                    updateHistoryTables(&PV->line[0], plyinfo.depth, &moves, res.bestIndex, position->toMove);
                }
        }
        PV->numMoves = 0;
        qmvStack.release();
        assert(stackCounter == qmvStack.getCounter());
        return res.alphabeta.alpha;
    }

    alphabeta = res.alphabeta;
    //no cutoff. Check whether we found a good move, mate/stalemate handling accordingly. Then Cleanup/bookkeeping, then return alpha
    //-------------------------------------------------

    //mate scores originate here!
    //------------------------------
    if (res.noMovesAvailable()) {
        if (sortinfo.movingSideInCheck) {
            alphabeta.mate(plyinfo.ply);
        } else {
            alphabeta.stalemate();
        }
    }

    //set hash entry
    //----------------------
    if (res.foundGoodMove()) {
        assert((res.bestIndex >= 0)&& (res.bestIndex < ((int32_t) moves.length)));
        updateHistoryTables(&PV->line[0], plyinfo.depth, &moves, res.bestIndex, position->toMove);
        searchCounts.bestIndex[plyinfo.depth][res.bestIndex]++;

        int16_t eval = alphabeta.alpha;
        bool isMateScore = (std::abs(eval) > 10000) && (std::abs(eval) < 30001);
        if (isMateScore) {
            if (eval < 0) {
                eval -= plyinfo.ply;
            } else {
                eval += plyinfo.ply;
            }
        }

        setHashEntry(FULLSEARCH, eval, plyinfo.depth, settings.searchId, (PV->line[0].sourceField | (PV->line[0].targetField << 8)), position->zobristHash);
        if (PV->line[0].captureType == none) {
                uint16_t toRemember = (PV->line[0].sourceField | (PV->line[0].targetField << 8));
                killerTable* table = getKillerTable();
                table->setKillerMove(plyinfo.ply, toRemember);
            }
    } else { //we failed low, remember as well
        setHashEntry(FAILLOW, alphabeta.alpha, plyinfo.depth, settings.searchId, 0, position->zobristHash);
    }

    //return memory
    //---------------------
    qmvStack.release();
    assert(stackCounter == qmvStack.getCounter());

    //and finally return alpha
    //--------------------------
    return alphabeta.alpha;
}

int16_t negamax(chessPosition* position, plyInfo plyinfo, AlphaBeta alphabeta, pvLine* PV, searchSettings settings) {
    alphabeta.sanityCheck();
    //check for timeout/interruption
    //------------------------------
    uint16_t numMoves = PV->numMoves;
    if ((plyinfo.depth > 3)&& (plyinfo.qply == 0)) { //TODO: be careful here - we may have to reset the stack in quiescence search as well!
        PV->numMoves = 0;
        checkTimeout();
    }
    PV->numMoves = numMoves;
    assert(alphabeta.alpha < alphabeta.beta);
    assert(plyinfo.ply+plyinfo.depth <= plyinfo.max_ply);

    searchCounts.called++;

    //50 move rule
    //-----------------------------
    if ((plyinfo.ply > 0) && (position->data.fiftyMoveRuleCounter >= 100)) {
        PV->numMoves = 0;
        return 0;
    }

    //repetition check. don't check repetition in root to avoid stupid losses. TODO: this is a quickfix and may have unintended consequences!
    //------------------------------------------------------------------------------------------
    if ((plyinfo.ply > 0) && (isRepetitionCandidate(position->zobristHash))) {
        if (backtrack_position_for_repetition(position)) {
            searchCounts.threefold_repetitions++;
            PV->numMoves = 0;
            return 0;
        } else {
            searchCounts.fake_3fold_repetitions++;
        }
    }

    //Interestingly, changing the order of the next two calls is a clear ELO loss - but why? I don't really get it.
    //go to quiescence on depth 0.
    //---------------------------
    if (plyinfo.depth <= 0) {
        searchCounts.wentToQuiescence++;
        PV->numMoves = 0;
        return negamaxQuiescence(position, plyinfo.qply, plyinfo.ply, alphabeta, 0, settings.searchId);
    }

    //check in hashtable for position value
    //----------------------------------------
    uint16_t hashmove = 0;
    int16_t hashEval = 0;
    uint16_t hashDepth = 0;

    if (checkHashTable(&hashEval, &hashmove, & hashDepth, settings.hashprobeSetting, position->zobristHash, &alphabeta.alpha, &alphabeta.beta, plyinfo.depth, plyinfo.ply, settings.searchId)) {
        PV->numMoves = 0;
        return hashEval;
    }

    uint64_t ownKing = position->pieceTables[position->toMove][king];
    bool movingSideInCheck = isFieldAttacked(position, (playerColor) (1-position->toMove), findLSB(ownKing));

    //futility pruning
    //-----------------
    if (plyinfo.depth == 1) {
        if (check_futility(movingSideInCheck, alphabeta.alpha, position, 100, 150)) {
            PV->numMoves = 0;
            return negamaxQuiescence(position, plyinfo.qply, plyinfo.ply, alphabeta, 0, settings.searchId);
        }
    }

    if (plyinfo.depth == 2) {
        if (check_futility(movingSideInCheck, alphabeta.alpha, position, 500, 600)) {
            PV->numMoves = 0;
            return negamaxQuiescence(position, plyinfo.qply, plyinfo.ply, alphabeta, 0, settings.searchId);
        }
    }

    //try nullmove pruning
    //-------------------------
    uint16_t refutationTarget = NO_REFUTATION; //invalid

    if ((settings.nullmoveSetting == nullmove_enabled) && !movingSideInCheck && (plyinfo.depth >= 2)) {
        if (check_nullmove(position, &refutationTarget, plyinfo.ply, plyinfo.max_ply, plyinfo.depth, alphabeta.beta, settings)) {
            PV->numMoves = 0;
            return alphabeta.beta;
        }
    }

    //IID
    //------
    if (((hashmove == 0) || hashDepth < 3) && (settings.hashprobeSetting == hashprobe_enabled) && (plyinfo.depth > 4)) {
        for (int16_t d = 3; d < plyinfo.depth; d++) {
            plyInfo info = plyinfo;
            info.depth = d;
            info.max_ply = info.ply+d+2;
            pvLine dummy;
            dummy.numMoves = 0;
            memset(&dummy.line[0], 0, sizeof(chessMove)); //important for debug vs release exactness!
            negamax(position, info, alphabeta, &dummy, settings);
            chessMove mv = dummy.line[0];
            hashmove = ((mv.sourceField) | (mv.targetField << 8));
        }
    }

    //------------------------------------------------------------------
    //now we are out of tricks, we need to start the actual search.
    //------------------------------------------------------------------
    return negamax_internal(position, plyinfo, alphabeta, PV, settings, sortInfo(movingSideInCheck, refutationTarget, hashmove));
}
