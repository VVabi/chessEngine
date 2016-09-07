/*
 * generateBishopMoveFields.cpp
 *
 *  Created on: Sep 7, 2016
 *      Author: vabi
 */



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
#include <lib/Defines/boardParts.hpp>

std::string generateBishopReachableFields() { //for magic number generation
	std::stringstream ss;
	ss << "uint64_t bishopFieldTable[] = {";

	for(int16_t ind=0; ind < 64; ind++){
		uint64_t bishopMoves = 0;

		int16_t ind_copy = 0;

		//NORTHEAST


		if(((ind+1) & 7) != 0) {
			ind_copy = ind+9;
			while((ind_copy < 56) && !((BIT64(ind_copy) & FILEH))) {
				bishopMoves = bishopMoves | BIT64(ind_copy);
				ind_copy = ind_copy+9;
			}
		}
		//NORTHWEST
		if(((ind) & 7) != 0) {
			ind_copy = ind+7;
			while(!((BIT64(ind_copy) & FILEA) ) && (ind_copy < 56)) {
				bishopMoves = bishopMoves | BIT64(ind_copy);
				ind_copy = ind_copy+7;
			}
		}
		//SOUTHEAST
		if(((ind+1) & 7) != 0) {
			ind_copy = ind-7;
			while((ind_copy > 7) && !((BIT64(ind_copy) & FILEH))) {
				bishopMoves = bishopMoves | BIT64(ind_copy);
				ind_copy = ind_copy-7;
			}
		}


		//SOUTHWEST
		if(((ind) & 7) != 0) {
			ind_copy = ind-9;
			while(!((BIT64(ind_copy) & FILEA)) && (ind_copy > 7)) {
				bishopMoves = bishopMoves | BIT64(ind_copy);
				ind_copy = ind_copy-9;
			}
		}

		ss << bishopMoves;

		if(ind < 63) {
			ss << " ,";
		}

	}

	ss << "}";


    return ss.str();
}


