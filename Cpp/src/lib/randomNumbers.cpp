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

uint64_t getRandUint64()
{
    std::random_device rd;

    std::mt19937_64 e2(rd());

    std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);

    return dist(e2);
}
