/*
 * externalParamReader.cpp
 *
 *  Created on: Feb 2, 2017
 *      Author: vabi
 */



#include "externalParamReader.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

parameterReader::parameterReader() {
    parameterFields = std::map<std::string, uint32_t>();
    parameterFields.insert(std::make_pair("pawnvalue", 0));
    parameterFields.insert(std::make_pair("knightvalue", 1));
    parameterFields.insert(std::make_pair("bishopvalue", 2));
    parameterFields.insert(std::make_pair("rookvalue", 3));
    parameterFields.insert(std::make_pair("queenvalue", 4));
    parameterFields.insert(std::make_pair("bishoppair", 5));
    parameterFields.insert(std::make_pair("rookonopenfile", 6));
    parameterFields.insert(std::make_pair("isolateddoublepawns", 7));
    parameterFields.insert(std::make_pair("nonisolateddoublepawns", 8));
    parameterFields.insert(std::make_pair("isolatedpawns", 9));
    parameterFields.insert(std::make_pair("selfopenfiletoking", 10));
    parameterFields.insert(std::make_pair("selfopenfilenexttoking", 11));
    parameterFields.insert(std::make_pair("opponentopenfiletoking", 12));
    parameterFields.insert(std::make_pair("opponentopenfilenexttoking", 13));
}


bool parameterReader::readParameters(preParameters* pars, std::string filename){

        std::ifstream input = std::ifstream(filename);
        if(!input.is_open()) {
            return false;
        }
        std::string line;
        int64_t* ptr = (int64_t*) pars;
        while(std::getline(input, line)) {
            std::stringstream line_stream(line);
            std::string key;
            line_stream >> key;
            //std::string lowercaseKey = std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            auto pair = parameterFields.find(key);

            if(pair != parameterFields.end()) {
                int64_t value;
                line_stream >> value;
                *(ptr+pair->second) = value;
            } else {
                std::cout << "Wrong parameter key" << std::endl;
            }

        }
        return true;
}

bool parameterReader::changeValue(preParameters* pars, std::string key, int64_t value) {
    auto pair = parameterFields.find(key);
    if(pair != parameterFields.end()) {
        int64_t* ptr = (int64_t*) pars;
        *(ptr+pair->second) = value;
        return true;
    } else {
        return false;
    }

}
