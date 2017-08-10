/*
 * randomNumbers.cpp
 *
 *  Created on: Sep 6, 2016
 *      Author: vabi
 */

#include <iostream>
#include <random>
#include <cmath>
#include <stdint.h>

static std::mt19937_64 e2 = std::mt19937_64(4288670196);

bool isInitialized = false;

uint64_t getRandUint64() {
    std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);
    return dist(e2);
}
