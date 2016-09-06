/*
 * magicNumberGeneration.hpp
 *
 *  Created on: Sep 6, 2016
 *      Author: vabi
 */

#ifndef MAGICNUMBERGENERATION_MAGICNUMBERGENERATION_HPP_
#define MAGICNUMBERGENERATION_MAGICNUMBERGENERATION_HPP_


std::string generateRookReachableFields();
uint64_t generateRookMagicNumber(uint16_t fieldIndex);
std::string generateRookMagicNumbers();
vdt_vector<vdt_vector<uint64_t>> generateRookMoveTables();
uint64_t generateRookMoveTable(uint16_t field, uint64_t blocker);
std::string generateRookMoveTablesString();
#endif /* MAGICNUMBERGENERATION_MAGICNUMBERGENERATION_HPP_ */
