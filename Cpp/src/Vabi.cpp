//============================================================================
// Name        : Vabi.cpp
// Author      : Fabian Lenhardt
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;
#include <lib/basics.hpp>
#include <lib/moveMaking/moveMaking.hpp>
#include <tests/tests.hpp>

int main() {
	testResult test = testMakeMove();
	std::cout << test.passed << std::endl;
	test = testUndoMove();
	std::cout << test.passed << std::endl;
	return 0;
}
