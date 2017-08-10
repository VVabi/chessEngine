/*
 * magicNumberGeneration.hpp
 *
 *  Created on: Sep 6, 2016
 *      Author: vabi
 */

#ifndef MAGICNUMBERGENERATION_MAGICNUMBERGENERATION_HPP_
#define MAGICNUMBERGENERATION_MAGICNUMBERGENERATION_HPP_
#include <string>

std::string generateRookReachableFields();
std::string generateBishopReachableFields();

uint64_t generateRookMagicNumber(uint16_t fieldIndex);
uint64_t generateBishopMagicNumber(uint16_t fieldIndex);

std::string generateRookMagicNumbers();
std::string generateBishopMagicNumbers();

vdt_vector<vdt_vector<uint64_t> > generateRookMoveTables();
uint64_t generateRookMoveTable(uint16_t field, uint64_t blocker);
std::string generateRookMoveTablesString();

vdt_vector<vdt_vector<uint64_t> > generateBishopMoveTables();
uint64_t generateBishopMoveTable(uint16_t field, uint64_t blocker);
std::string generateBishopMoveTablesString();
#endif /* MAGICNUMBERGENERATION_MAGICNUMBERGENERATION_HPP_ */
