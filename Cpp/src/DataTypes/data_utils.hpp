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

#define SAFE_ARRAY_ACCESS(x,n) safe_access<std::remove_reference<decltype(x[0])>::type, sizeof(x)/sizeof(x[0])>(x,n)

#endif /* DATATYPES_DATA_UTILS_HPP_ */
