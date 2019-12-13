//
// Created by erik on 13-12-19.
//

#ifndef SIMPLELUA_METATABLE_H
#define SIMPLELUA_METATABLE_H

class MetaTable {
public:
    static std::unordered_map<const std::type_info*, std::string> metatables;
};



#endif //SIMPLELUA_METATABLE_H
