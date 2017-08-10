/*
 * history.cpp
 *
 *  Created on: Oct 16, 2016
 *      Author: vabi
 */



#include <stdint.h>
#include <iostream>
#include "history.hpp"

static HistoryTables historyTables;


HistoryTables* getHistoryTables() {
    return &historyTables;
}

