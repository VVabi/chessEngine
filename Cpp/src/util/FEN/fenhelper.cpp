/*
 * fenhelper.cpp
 *
 *  Created on: Nov 25, 2018
 *      Author: vabi
 */

#include <stdint.h>
#include <hashTables/hashTables.hpp>
#include <lib/basics.hpp>
#include <lib/basicTypes.hpp>
#include <lib/DebugFunctions/debugFunctions.hpp>
#include <lib/figureValueHashing.hpp>
#include <Search/repetition.hpp>
#include <iostream>
#include <string>

static char figureNames[2][6] = { {'P', 'N', 'B', 'R', 'Q', 'K'},
        {'p', 'n', 'b', 'r', 'q', 'k'},
};


std::string chessPositionToString(const chessPosition position) {
    //Not performance-critical
    //---------------------------
    std::string ret = "";

    for (uint8_t ind = 0; ind < 64; ind++) {
        uint64_t num = (1ULL << ind);
        bool found = false;
        for (uint8_t color = 0; color < 2; color++) {
            for (uint8_t figureCnt = 0; figureCnt < 6; figureCnt++) {
                if (position.pieceTables[color][figureCnt] & num) {
                    ret.push_back(figureNames[color][figureCnt]);
                    found = true;
                }
            }
        }

        if (!found) {
            ret.push_back('0');
        }
    }

    if (position.toMove == white) {
        ret.push_back('w');
    } else {
        ret.push_back('b');
    }

    uint8_t castlingRights = position.data.castlingRights;

    if (castlingRights & 1) {
        ret.push_back('K');
    } else {
        ret.push_back('0');
    }

    if (castlingRights & 2) {
        ret.push_back('Q');
    } else {
        ret.push_back('0');
    }

    if (castlingRights & 4) {
        ret.push_back('k');
    } else {
        ret.push_back('0');
    }

    if (castlingRights & 8) {
        ret.push_back('q');
    } else {
        ret.push_back('0');
    }


    return ret;
}

chessPosition FENtoChessPosition(std::string fen) {
    repetitionData.reset(); //TODO: I dont like this.
    //Not performance-critical
    //---------------------------
    chessPosition position;
    zeroInitPosition(&position);

    //r1bqkb1r/pp3ppp/2np1n2/1N2p3/4P3/2N5/PPP2PPP/R1BQKB1R w KQkq - 0 7

    uint16_t ind = 0;
    uint16_t row = 7;
    uint16_t file = 0;
    while (fen.at(ind) != ' ') {
        char current = fen.at(ind);

        if (current == '/') {
            row--;
            file = 0;
            ind++;
            continue;
        }

        if ((current >= '0') && (current <= '8')) {
            uint8_t num = current-'0';
            file = file+num;
        } else {
            uint16_t field = 8*row+file;
            file++;
            switch (current) {
                case 'K':
                    position.pieces[white]                  |= (1ULL << field);
                    position.pieceTables[white][king]       |= (1ULL << field);
                    break;
                case 'Q':
                    position.pieceTables[white][queen]      |= (1ULL << field);
                    position.pieces[white]                  |= (1ULL << field);
                    break;
                case 'R':
                    position.pieceTables[white][rook]       |= (1ULL << field);
                    position.pieces[white]                  |= (1ULL << field);
                    break;
                case 'B':
                    position.pieceTables[white][bishop]     |= (1ULL << field);
                    position.pieces[white]                  |= (1ULL << field);
                    break;
                case 'N':
                    position.pieceTables[white][knight]     |= (1ULL << field);
                    position.pieces[white]                  |= (1ULL << field);
                    break;
                case 'P':
                    position.pieceTables[white][pawn]       |= (1ULL << field);
                    position.pieces[white]                  |= (1ULL << field);
                    break;
                case 'k':
                    position.pieceTables[black][king]       |= (1ULL << field);
                    position.pieces[black]                  |= (1ULL << field);
                    break;
                case 'q':
                    position.pieceTables[black][queen]      |= (1ULL << field);
                    position.pieces[black]                  |= (1ULL << field);
                    break;
                case 'r':
                    position.pieceTables[black][rook]       |= (1ULL << field);
                    position.pieces[black]                  |= (1ULL << field);
                    break;
                case 'b':
                    position.pieceTables[black][bishop]     |= (1ULL << field);
                    position.pieces[black]                  |= (1ULL << field);
                    break;
                case 'n':
                    position.pieceTables[black][knight]     |= (1ULL << field);
                    position.pieces[black]                  |= (1ULL << field);
                    break;
                case 'p':
                    position.pieceTables[black][pawn]       |= (1ULL << field);
                    position.pieces[black]                  |= (1ULL << field);
                    break;
                default:
                    std::cout << "invalid fen  position string " << current << std::endl;
                    break;
            }
        }
        ind++;
    }

    ind++;
    if (fen.at(ind) == 'w') {
        position.toMove = white;
    } else {
        position.toMove = black;
    }
    ind++; //should be a space now
    position.data.castlingRights = 0;
    while ((fen.at(ind) == ' ') && ((ind+1) < static_cast<uint16_t>(fen.size()))) {
        ind++;
    }

    while (fen.at(ind) != ' ') {
        if (fen.at(ind) == 'K') {
            position.data.castlingRights = position.data.castlingRights | 1;
        }
        if (fen.at(ind) == 'Q') {
            position.data.castlingRights = position.data.castlingRights | 2;
        }
        if (fen.at(ind) == 'k') {
            position.data.castlingRights = position.data.castlingRights | 4;
        }
        if (fen.at(ind) == 'q') {
            position.data.castlingRights = position.data.castlingRights | 8;
        }
        ind++;
    }

    while ((fen.at(ind) == ' ') && ((ind+1) < static_cast<uint16_t>(fen.size()))) {
        ind++;
    }
    position.data.enPassantFile = 8;
    position.data.fiftyMoveRuleCounter = 0;

    if (fen.at(ind) != '-') {
        position.data.enPassantFile = fen.at(ind)-'a';
        ind++;
    }

    position.totalFigureEval   = calcTotalFigureEvaluation(&position);
    position.figureEval   = calcFigureEvaluation(&position);
    position.pieceTableEval = ((1 << 15) + position.figureEval+ calcPieceTableValue(&position))
                            +(((1 << 14) + calcEndGamePieceTableValue(&position)+position.figureEval) << 16);
    position.zobristHash    = calcZobristHash(&position);
    position.pawnHash       = calcPawnHash(&position);
    position.data.hash = position.zobristHash;
    position.presentPieces = calcPresentPieces(&position);
    return position;
}

std::string chessPositionToFenString(const chessPosition position, bool EPD) {
    std::string str = chessPositionToString(position);
    std::string FEN = "";

    for (int16_t row = 7; row > -1; row--) {
        uint16_t empty = 0;

        for (uint16_t column = 0; column < 8; column++) {
            uint8_t field = 8*row+column;
            char current = str.at(field);
            if (current == '0') {
                empty++;
            } else {
                if (empty > 0) {
                    FEN = FEN+std::to_string(empty);
                    empty = 0;
                }
                FEN.push_back(current);
            }
        }
        if (empty > 0) {
            FEN = FEN+std::to_string(empty);
        }
        if (row > 0) {
            FEN.push_back('/');
        }
    }


    FEN.push_back(' ');

    FEN.push_back(str.at(64));

    FEN.push_back(' ');


    uint8_t castling = position.data.castlingRights;

    if (castling & 1) {
        FEN.push_back('K');
    }
    if (castling & 2) {
        FEN.push_back('Q');
    }
    if (castling & 4) {
        FEN.push_back('k');
    }
    if (castling & 8) {
        FEN.push_back('q');
    }
    if (!castling) {
        FEN.push_back('-');
    }

    FEN.push_back(' ');

    if (position.data.enPassantFile > 7) {
        FEN.push_back('-');
    } else {
        if (position.toMove == white) { //BLACK made the ep move
            uint16_t field = 40+position.data.enPassantFile;
            if (EPD) {
                char file = (field & 7)+'a';
                char row  = (field/8)+'0';
                FEN.push_back(file);
                FEN.push_back(row);

            } else {
                FEN = FEN+std::to_string(field);
            }

        } else { //WHITE made the ep move
            uint16_t field = 16+position.data.enPassantFile;
            if (EPD) {
                char file = (field & 7)+'a';
                char row  = (field/8)+'0';
                FEN.push_back(file);
                FEN.push_back(row);

            } else {
                FEN = FEN+std::to_string(field);
            }
        }
    }

    if (!EPD) {
        FEN.push_back(' ');
        FEN.push_back('0');
        FEN.push_back(' ');

        uint16_t moves = position.madeMoves.length;
        FEN = FEN+std::to_string(moves+1);
    }
    return FEN;
}
