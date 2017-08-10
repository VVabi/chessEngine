/*
 * fsarray.cpp
 *
 *  Created on: Sep 19, 2015
 *      Author: fabian
 */

#ifndef CPP_SRC_DATATYPES_FSARRAY_H_
#define CPP_SRC_DATATYPES_FSARRAY_H_
#include <stdint.h>
#include <stddef.h>
#include <iostream>
#include <string.h>
template<typename T> class fsarray {
 public:
    uint32_t length;
    T* data;
    fsarray() {
        length = 0;
        data = 0;
    }
    explicit fsarray(int n) {
        length = n;
        if (length > 0) {
            data = new T[length];
        } else {
            data = 0;
        }
    }

    T &operator[](unsigned int i) {
        /*if (i > length) {

        }*/
        return data[i];
    }

    void free_array() {
        if (data != NULL) {
            delete[] data;
            data = NULL;
            length = 0;
        }
    }

    fsarray<T> vdt_copy() {
        fsarray<T> ret = fsarray<T>(length);
        for (uint32_t i = 0; i < length; i++) {
            ret[i] = data[i].copy();
        }
        return ret;
    }

    fsarray<T> shallow_copy() {
        fsarray<T> ret = fsarray<T>(length);
        memcpy(ret.data, data, length*sizeof(T));
        return ret;
    }
};
#endif  // CPP_SRC_DATATYPES_FSARRAY_H_
