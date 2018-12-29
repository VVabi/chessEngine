// ============================================================================
// Name        : Vabi.cpp
// Author      : Fabian Lenhardt
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
// ============================================================================

#include <iostream>
#include <lib/basics.hpp>
#include <fstream>
#include <tr1/memory>
#include <lib/Attacks/attacks.hpp>
#include <lib/bitfiddling.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <Search/search.hpp>
#include <hashTables/hashTables.hpp>
#include <userInterface/UIlayer.hpp>
#include <sstream>
#include <tests/tests.hpp>
#include <parameters/parameters.hpp>

int main() {
    initializeParameters();
    //srand(get_timestamp());
#ifdef EXPERIMENTAL
    srand(get_timestamp()+153);
#else
    srand(get_timestamp());
#endif
    initZobristHashValues();
    initHashTables();

    UIloop();
}

