/*
 * repetition.hpp
 *
 *  Created on: Aug 11, 2017
 *      Author: vabi
 */

#ifndef SEARCH_REPETITION_HPP_
#define SEARCH_REPETITION_HPP_
#include <stdint.h>
#include <string.h>
#include <assert.h>
#define REPTABLESIZE 16384

class RepetitionData {
    uint16_t repetitionData[REPTABLESIZE] = {0};

 public:
    void reset() {
        memset(repetitionData, 0, REPTABLESIZE*sizeof(uint16_t));
    }

    void increment(uint64_t zobristHash) {
        uint16_t index = zobristHash & (REPTABLESIZE-1);
        repetitionData[index]++;
    }

    void decrement(uint64_t zobristHash) {
        uint16_t index = zobristHash & (REPTABLESIZE-1);
        assert(repetitionData[index] > 0);
        repetitionData[index]--;
    }

    bool isRepetitionCandidate(uint64_t zobristHash) const{
        uint16_t index = zobristHash & (REPTABLESIZE-1);
        return (repetitionData[index] > 0);
    }
};

extern RepetitionData repetitionData;

__attribute__((always_inline)) static inline bool isRepetitionCandidate(uint64_t zobristHash) {
    return repetitionData.isRepetitionCandidate(zobristHash);
}

__attribute__((always_inline)) static inline void repetitionIncrement(uint64_t zobristHash) {
    repetitionData.increment(zobristHash);
}

__attribute__((always_inline)) static inline void repetitionDecrement(uint64_t zobristHash) {
    return repetitionData.decrement(zobristHash);
}
#endif /* SEARCH_REPETITION_HPP_ */
