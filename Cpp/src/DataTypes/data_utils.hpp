/*
 * data_utils.hpp
 *
 *  Created on: Dec 29, 2018
 *      Author: vabi
 */

#ifndef DATATYPES_DATA_UTILS_HPP_
#define DATATYPES_DATA_UTILS_HPP_
#include <lib/basics.hpp>
#include <cstdint>

template<typename T, uint32_t length> inline T safe_access(T* val, uint32_t index) {
#ifdef DEBUG
    if (index >= length) {
        logError("Index out of bounds");
        while(true) {}
    }
#endif
    return val[index];
}



#endif /* DATATYPES_DATA_UTILS_HPP_ */
