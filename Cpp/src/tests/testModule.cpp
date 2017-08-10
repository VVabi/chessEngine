/*
 * testModule.cpp
 *
 *  Created on: Sep 24, 2016
 *      Author: vabi
 */


#include <iostream>
#include "tests.hpp"

typedef testResult (*testFunction)( void );

static testFunction funcs[] = { testMakeMove, testUndoMove, testPerftTestSuite};


int numTests = 3;

void runTests() {
    for(uint16_t ind=0; ind < numTests; ind++){
        testResult ret = funcs[ind]();
        std::cout << "Test " << ret.testName << " ";
        if(ret.passed) {
            std::cout << "PASSED" << std::endl;
        } else {
            std::cout << "FAILED" << std::endl;
        }
    }
}

