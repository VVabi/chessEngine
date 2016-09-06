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
extern uint64_t rookFieldTable[];
extern uint64_t rookMagicNumbers[];


std::string generateRookMoveTablesString(){
	vdt_vector<vdt_vector<uint64_t>> vec =  generateRookMoveTables();

	std::stringstream ss;
	ss << std::hex;
	ss << "uint64_t rookMoveTables[][] = {\n";

	for(int i=0; i < 64; i++) {
		ss << "{";
		for(int j=0; j<4096; j++) {
			ss << "0x" << vec[i][j];

			if(j < 4095){
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

uint64_t generateRookMoveTable(uint16_t field, uint64_t blocker){
	uint64_t rookMoveTable = 0;
	int16_t f_copy        = field-8;
	while(f_copy >= 0){
		 rookMoveTable =  rookMoveTable | BIT64(f_copy);
		 if(BIT64(f_copy) & blocker){
			 break;
		 }
		 f_copy = f_copy-8;
	}
	f_copy = field+8;
	while(f_copy < 64){
		 rookMoveTable =  rookMoveTable | BIT64(f_copy);
		 if(BIT64(f_copy) & blocker){
			 break;
		 }
		 f_copy = f_copy+8;
	}
	f_copy = field+1;
	while( (f_copy >> 3) == (field >> 3)) {
		rookMoveTable =  rookMoveTable | BIT64(f_copy);
		 if(BIT64(f_copy) & blocker){
			 break;
		 }
		f_copy = f_copy+1;
	}
	f_copy = field-1;
	if(f_copy >= 0) {
		while( (f_copy >> 3) == (field >> 3)) {
			rookMoveTable =  rookMoveTable | BIT64(f_copy);
			 if(BIT64(f_copy) & blocker){
				 break;
			 }
			f_copy = f_copy-1;
		}
	}

	return rookMoveTable;
}

 vdt_vector<vdt_vector<uint64_t>> generateRookMoveTables(){
	 vdt_vector<vdt_vector<uint64_t>> ret = vdt_vector<vdt_vector<uint64_t>>(64);
	for(uint16_t field=0; field < 64; field++) {
		uint64_t magicNumber = rookMagicNumbers[field];
		vdt_vector<uint64_t> fieldMoveTables = vdt_vector<uint64_t>(4096);
		uint64_t dummy = 0;
		for(uint16_t ind=0; ind < 4096; ind++) {
			fieldMoveTables.add(&dummy);
		}
		uint16_t numFieldsReachable = popcount(rookFieldTable[field]);
		uint16_t numVariations      = (1 << numFieldsReachable);
		for(uint16_t ind=0; ind < numVariations; ind++) {
				uint64_t blocker = 0;
				uint16_t cnt = 0;
				for(uint16_t i=0; i < 64; i++){
					if(rookFieldTable[field] & (1UL << i)) {

						if(ind & (1 << cnt)){
							blocker = blocker | (1UL << i);
						}
						cnt++;
					}
				}
				uint64_t hashValue = ((blocker*magicNumber) >> 52);
				fieldMoveTables[hashValue] = generateRookMoveTable(field, blocker);
			}


		ret.add(&fieldMoveTables);
	}

	return ret;
}



std::string generateRookMagicNumbers(){
	std::stringstream ss;
	ss << std::hex;
	ss << "uint64_t rookMagicNumbers[] = {";
	for (uint16_t ind=0; ind < 64; ind++) {
		std::cout << "Currently at " << ind << std::endl;
		ss << "0x";
		ss << generateRookMagicNumber(ind);

		if(ind < 63) {
			ss << " ,";
		}
	}

	ss << "}";
	return ss.str();
}

uint64_t generateRookMagicNumber(uint16_t fieldIndex) {
	bool foundMagic = false;
	uint64_t magicNumber = 0;

	uint64_t blockers[4096];

	uint16_t numFieldsReachable = popcount(rookFieldTable[fieldIndex]);
	uint16_t numVariations      = (1 << numFieldsReachable);
	for (uint16_t ind=0; ind < numVariations; ind++) {
		uint64_t blocker = 0;
		uint16_t cnt = 0;
		for(uint16_t i=0; i < 64; i++){
			if(rookFieldTable[fieldIndex] & (1UL << i)) {

				if(ind & (1 << cnt)){
					blocker = blocker | (1UL << i);
				}
				cnt++;
			}
		}
		blockers[ind] = blocker;
	}

	while(!foundMagic) {

		magicNumber = getRandUint64() & getRandUint64() & getRandUint64();
		bool indexCheck[4096] = {false};
		foundMagic = true;
		for(uint16_t ind=0; ind < numVariations; ind++) {
			uint16_t val = ((blockers[ind]*magicNumber) >> 52);

			if(indexCheck[val] == true){
				foundMagic = false;
				break;
			}
			indexCheck[val] = true;

		}
	}


	return magicNumber;
}
