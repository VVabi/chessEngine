/*
 * figureValueHashing.hpp
 *
 *  Created on: Aug 27, 2017
 *      Author: vabi
 */

#ifndef LIB_FIGUREVALUEHASHING_HPP_
#define LIB_FIGUREVALUEHASHING_HPP_

#include <iostream>
#include "basicTypes.hpp"

class PresentPieces {
    uint64_t value = 0;

 public:
    void addPiece(playerColor color, figureType type) {
        if (type != none) {
            uint16_t offset = 32*color+4*type;
            value = value+BIT64(offset);
        }
    }

    void removePiece(playerColor color, figureType type) {
        if (type != none) {
            uint16_t offset = 32*color+4*type;
    #ifdef DEBUG
            uint64_t total = BIT64(offset) | BIT64(offset+1) | BIT64(offset+2) | BIT64(offset+3);
            if ((total & value) == 0) {
                std::cout << "ERROR: Removing nonexistent piece from present pieces??" << std::endl;
            }
    #endif
            value = value-BIT64(offset);
        }
    }

    void reset() {
        value = 0;
    }

    uint64_t getInternalValue() const { //for debugging purposes only
        return value;
    }

    bool compare(uint64_t other) const {
        return (other == value);
    }
};


#endif /* LIB_FIGUREVALUEHASHING_HPP_ */
