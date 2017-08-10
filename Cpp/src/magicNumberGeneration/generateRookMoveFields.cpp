/*
 * generateRookMoveFields.cpp
 *
 *  Created on: Sep 6, 2016
 *      Author: vabi
 */


#include <string>
#include <stdint.h>
#include <sstream>      // std::stringstream
#include <lib/basics.hpp>

std::string generateRookReachableFields() { //for magic number generation
    std::stringstream ss;
    ss << "uint64_t rookFieldTable[] = {";

    for (int16_t ind = 0; ind < 64; ind++) {
        uint64_t rookMoves = 0;

        int16_t ind_copy = 0;
        ind_copy = ind-8;
        while (ind_copy > 7) {
            rookMoves = rookMoves | BIT64(ind_copy);
            ind_copy = ind_copy-8;
        }
        ind_copy = ind+8;
        while (ind_copy < 56) {
            rookMoves = rookMoves | BIT64(ind_copy);
            ind_copy = ind_copy+8;
        }

        if (((ind+1) & 7) != 0) {
            ind_copy = ind+1;
            while (((ind_copy >> 3) == (ind >> 3)) && (((ind_copy+1) & 7) != 0)) {
                rookMoves =  rookMoves| BIT64(ind_copy);
                ind_copy = ind_copy+1;
            }
        }
        if ((ind & 7) != 0) {
            ind_copy = ind-1;
            while (((ind_copy >> 3) == (ind >> 3)) && ((ind_copy & 7) != 0)) {
                rookMoves =  rookMoves| BIT64(ind_copy);
                ind_copy = ind_copy-1;
            }
        }

        ss << rookMoves;

        if (ind < 63) {
            ss << " , ";
        }
    }
    ss << "}";
    return ss.str();
}



