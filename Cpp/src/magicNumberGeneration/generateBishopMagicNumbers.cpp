/*
 * generateBishopMagicNumbers.cpp
 *
 *  Created on: Sep 7, 2016
 *      Author: vabi
 */


/*
 * generateRookMagicNumbers.cpp
 *
 *  Created on: Sep 6, 2016
 *      Author: vabi
 */


#include <iostream>
#include <lib/basics.hpp>
#include <magicNumberGeneration/magicNumberGeneration.hpp>
#include <sstream>      // std::stringstream
#include <DataTypes/vdt_vector.hpp>
#include <lib/bitfiddling.h>
#include <lib/Defines/boardParts.hpp>
extern const uint64_t bishopFieldTable[];
extern const uint64_t bishopMagicNumbers[];



uint64_t generateBishopMoveTable(uint16_t field, uint64_t blocker){
	uint64_t bishopMoveTable = 0;

	//NORTHEAST
	int16_t f_copy        = field+9;
	while((f_copy < 64) && (!(BIT64(f_copy) & FILEA))){
		 bishopMoveTable =  bishopMoveTable | BIT64(f_copy);
		 if(BIT64(f_copy) & blocker){
			 break;
		 }
		 f_copy = f_copy+9;
	}

	//NORTHWEST
	f_copy = field+7;
	while((f_copy < 64) && (!(BIT64(f_copy) & FILEH))){
		 bishopMoveTable =  bishopMoveTable | BIT64(f_copy);
		 if(BIT64(f_copy) & blocker){
			 break;
		 }
		 f_copy = f_copy+7;
	}

	//SOUTHEAST
	f_copy = field-7;
	while((f_copy >= 0) && (!(BIT64(f_copy) & FILEA))){
		bishopMoveTable =  bishopMoveTable | BIT64(f_copy);
		 if(BIT64(f_copy) & blocker){
			 break;
		 }
		f_copy = f_copy-7;
	}

	//SOUTHWEST
	f_copy = field-9;
	if(f_copy >= 0) {
		while((f_copy >= 0) && (!(BIT64(f_copy) & FILEH))){
			bishopMoveTable =  bishopMoveTable | BIT64(f_copy);
			 if(BIT64(f_copy) & blocker){
				 break;
			 }
			f_copy = f_copy-9;
		}
	}

	return bishopMoveTable;
}

 vdt_vector<vdt_vector<uint64_t> > generateBishopMoveTables(){
	 vdt_vector<vdt_vector<uint64_t> > ret = vdt_vector<vdt_vector<uint64_t> >(64);
	for(uint16_t field=0; field < 64; field++) {
		uint64_t magicNumber = bishopMagicNumbers[field];
		vdt_vector<uint64_t> fieldMoveTables = vdt_vector<uint64_t>(512);
		uint64_t dummy = 0;
		for(uint16_t ind=0; ind < 512; ind++) {
			fieldMoveTables.add(&dummy);
		}
		uint16_t numFieldsReachable = popcount(bishopFieldTable[field]);
		uint16_t numVariations      = (1 << numFieldsReachable);
		for(uint16_t ind=0; ind < numVariations; ind++) {
				uint64_t blocker = 0;
				uint16_t cnt = 0;
				for(uint16_t i=0; i < 64; i++){
					if(bishopFieldTable[field] & (1ULL << i)) {

						if(ind & (1 << cnt)){
							blocker = blocker | (1ULL << i);
						}
						cnt++;
					}
				}
				uint64_t hashValue = ((blocker*magicNumber) >> 55);
				fieldMoveTables[hashValue] = generateBishopMoveTable(field, blocker);
			}


		ret.add(&fieldMoveTables);
	}

	return ret;
}

 std::string generateBishopMoveTablesString(){
 	vdt_vector<vdt_vector<uint64_t> > vec =  generateBishopMoveTables();

 	std::stringstream ss;
 	ss << std::hex;
 	ss << "uint64_t bishopMoveTables[][] = {\n";

 	for(int i=0; i < 64; i++) {
 		ss << "{";
 		for(int j=0; j<512; j++) {
 			ss << "0x" << vec[i][j];

 			if(j < 511){
 				ss << " ,";
 			}

 		}


 		ss << "}";
 		if(i < 63) {
 			ss << ",";
 		}
 		ss << "\n";
 	}
 	ss << "};";

 	return ss.str();

 }


uint64_t generateBishopMagicNumber(uint16_t fieldIndex) {
	bool foundMagic = false;
	uint64_t magicNumber = 0;

	uint64_t blockers[512];

	uint16_t numFieldsReachable = popcount(bishopFieldTable[fieldIndex]);

	if(numFieldsReachable > 9){
		std::cout << "WTF???" << std::endl;
	}
	uint16_t numVariations      = (1 << numFieldsReachable);
	for (uint16_t ind=0; ind < numVariations; ind++) {
		uint64_t blocker = 0;
		uint16_t cnt = 0;
		for(uint16_t i=0; i < 64; i++){
			if(bishopFieldTable[fieldIndex] & (1ULL << i)) {

				if(ind & (1 << cnt)){
					blocker = blocker | (1ULL << i);
				}
				cnt++;
			}
		}
		blockers[ind] = blocker;
	}

	while(!foundMagic) {

		magicNumber = getRandUint64() & getRandUint64() & getRandUint64();
		bool indexCheck[512] = {false};
		foundMagic = true;
		for(uint16_t ind=0; ind < numVariations; ind++) {
			uint16_t val = ((blockers[ind]*magicNumber) >> 55);

			if(indexCheck[val] == true){
				foundMagic = false;
				break;
			}
			indexCheck[val] = true;

		}
	}


	return magicNumber;
}

std::string generateBishopMagicNumbers(){
	std::stringstream ss;
	ss << std::hex;
	ss << "uint64_t bishopMagicNumbers[] = {";
	for (uint16_t ind=0; ind < 64; ind++) {
		std::cout << "Currently at " << ind << std::endl;
		ss << "0x";
		ss << generateBishopMagicNumber(ind);

		if(ind < 63) {
			ss << " ,";
		}
	}

	ss << "}";
	return ss.str();
}

