/*
 * selfPlaying.hpp
 *
 *  Created on: Sep 25, 2016
 *      Author: vabi
 */

#ifndef SELFPLAYING_SELFPLAYING_HPP_
#define SELFPLAYING_SELFPLAYING_HPP_

#include <lib/basics.hpp>

struct selfPlayResult{
    uint32_t numGames;
    uint32_t engine1Wins;
    uint32_t engine2Wins;
    uint32_t draws;
};

selfPlayResult playSelf(uint16_t depth1, uint16_t depth2, uint32_t numGames);

#endif /* SELFPLAYING_SELFPLAYING_HPP_ */
