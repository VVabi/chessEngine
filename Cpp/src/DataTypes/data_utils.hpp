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

#define SAFE_ARR_ACC(x,n) safe_access<std::remove_reference<decltype(x[0])>::type, sizeof(x)/sizeof(x[0])>(x,n) //TODO: unfortunately does not work as intended since decltype(x[0]) returns a reference.

#endif /* DATATYPES_DATA_UTILS_HPP_ */
