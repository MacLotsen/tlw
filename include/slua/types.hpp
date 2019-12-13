//
// Created by enijenhuis on 11-12-2019.
//

#ifndef SIMPLELUA_TYPES_HPP
#define SIMPLELUA_TYPES_HPP

#include <string>
#include <vector>
#include <map>
#include "value.hpp"

typedef bool Boolean;
typedef double Number;
typedef std::string String;
typedef std::vector<AbstractValue *> List;
typedef std::map<std::string, AbstractValue *> Table;
typedef void *Class;

#endif //SIMPLELUA_TYPES_HPP
