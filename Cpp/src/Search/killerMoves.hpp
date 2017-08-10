/*
 * killerMoves.hpp
 *
 *  Created on: Aug 10, 2017
 *      Author: vabi
 */

#ifndef SEARCH_KILLERMOVES_HPP_
#define SEARCH_KILLERMOVES_HPP_

#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <iostream>

#define MAX_PLY 50
#define NUM_KILLERS_PER_PLY 2

struct singlePlyKillers {
    uint16_t killers[NUM_KILLERS_PER_PLY];
};

class killerTable {
    singlePlyKillers killerMoves[MAX_PLY];

 public:
    void clear() {
        memset(killerMoves, 0, sizeof(singlePlyKillers)*MAX_PLY);
    }

    void setKillerMove(uint16_t ply, uint16_t toRemember) {
        assert(ply < MAX_PLY);
        assert(toRemember != 0);
        if (killerMoves[ply].killers[0] != toRemember) {
            for (uint16_t ind = NUM_KILLERS_PER_PLY-1; ind > 0; ind--) {
                killerMoves[ply].killers[ind] = killerMoves[ply].killers[ind-1];
            }
            killerMoves[ply].killers[0] = toRemember;
        }
    }

    const singlePlyKillers getKillers(uint16_t ply) {
        return killerMoves[ply];
    }
};


killerTable* getKillerTable();
#endif /* SEARCH_KILLERMOVES_HPP_ */
