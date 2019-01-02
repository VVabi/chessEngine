#include <hashTables/hashTables.hpp>
#include <parameters/parameters.hpp>
#include <userInterface/UIlayer.hpp>
#include <cstdlib>

int main() {
    initializeParameters();
#ifdef EXPERIMENTAL
    srand(get_timestamp()+153);
#else
    srand(get_timestamp());
#endif
    initZobristHashValues();
    initHashTables();

    UIloop();
}

