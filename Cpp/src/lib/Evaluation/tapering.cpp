/*
 * tapering.cpp
 *
 *  Created on: Aug 11, 2017
 *      Author: vabi
 */
#include "tapering.hpp"


static uint16_t taperingValues[81] = {  0,  0,  0,  0,  0,  0,  0,  0,
                                     0,  0,  0,  0,  0,  0,  0,  0,
                                    0, 2, 7, 12, 17, 22, 27, 33,
                                    39, 45, 51, 57, 64, 71, 78, 85,
                                    92, 99, 106, 113, 120, 127, 134, 141,
                                   148, 155, 162, 168, 174, 180, 186, 192,
                                   197, 202, 206, 210, 214, 218, 222, 226,
                                   229, 232, 235, 238, 241, 244, 247, 256,
                                   256, 256, 256, 256, 256, 256, 256, 256,
                                   256, 256, 256, 256, 256, 256, 256, 256, 256
    };


uint16_t getTaperingValue(uint16_t material) {
   if (material > 80) {
       material = 80;
   }
   return taperingValues[material];
}
