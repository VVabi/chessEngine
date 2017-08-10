/*
 * externalParamReader.hpp
 *
 *  Created on: Feb 2, 2017
 *      Author: vabi
 */

#include <stdint.h>
#include <string>
#include <vector>
#include "parametersPrivate.hpp"
#include <map>
class parameterReader {
 private:
    std::map<std::string, uint32_t> parameterFields;
 public:
    parameterReader();
    bool readParameters(preParameters* pars, std::string filename);
    bool changeValue(preParameters* pars, std::string key, int64_t value);
};
