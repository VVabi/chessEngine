/*
 * history.hpp
 *
 *  Created on: Aug 6, 2017
 *      Author: vabi
 */

#ifndef SEARCH_HISTORY_HPP_
#define SEARCH_HISTORY_HPP_

#include <stdint.h>
#include <lib/basics.hpp>

class HistoryTables {
    int32_t historyTable[2][64][64];
    int32_t higherCutoff = (1 << 12);
    int32_t lowerCutoff  = -(1 << 12);
public:
    void clearHistoryTable() {
        for(uint16_t color=0; color < 2; color++) {
            for(uint16_t from=0; from < 64; from++) {
                for(uint16_t to=0; to<64; to++) {
                    historyTable[color][from][to] = 0;
                }
            }
        }
    }

    void rescale() {
        for(uint16_t color=0; color < 2; color++) {
            for(uint16_t from=0; from < 64; from++) {
                for(uint16_t to=0; to<64; to++) {
                    historyTable[color][from][to] = historyTable[color][from][to]/4;
                }
            }
        }
    }

    int32_t getHistoryEntry(playerColor color, uint16_t source, uint16_t target) {
        return historyTable[color][source][target];
    }

    void changeEntry(playerColor color, uint16_t source, uint16_t target, int32_t change) {
        historyTable[color][source][target] += change;
        if ((historyTable[color][source][target] > higherCutoff) || (historyTable[color][source][target] < lowerCutoff)) {
            rescale();
        }
    }

    HistoryTables() {
        clearHistoryTable();
    }
};

HistoryTables* getHistoryTables();

#endif /* SEARCH_HISTORY_HPP_ */
