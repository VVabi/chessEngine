/*
 * attacks.hpp
 *
 *  Created on: Sep 19, 2016
 *      Author: vabi
 */

#ifndef LIB_ATTACKS_ATTACKS_HPP_
#define LIB_ATTACKS_ATTACKS_HPP_
struct AttackTable{
    uint64_t attackTables[6];
    uint64_t completeAttackTable;
};



bool isFieldAttacked(const chessPosition* position, playerColor attackingSide, uint16_t field);

AttackTable makeAttackTable(const chessPosition* position, playerColor attackingSide, uint64_t ignoreMask = 0);
AttackTable makeAttackTableWithMobility(const chessPosition* position, playerColor attackingSide, int16_t* mobilityScore);
int16_t SEE(chessPosition* position, chessMove* mv);
int16_t SEEold(chessPosition* position, chessMove* mv);
#endif /* LIB_ATTACKS_ATTACKS_HPP_ */
