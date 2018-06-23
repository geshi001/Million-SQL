#pragma once
#include <string>
#include <vector>

enum class AttrType { INT, FLOAT, CHAR };

struct Attribute {
    AttrType attrType; // "char", "int", "float"
    std::string attrName;
    size_t charCnt;
    bool isUnique;
};

struct Value {
    AttrType attrType;
    union {
        int ival;
        float fval;
        char cval[256];
    };
};

enum class OpType { EQ, NE, LT, LEQ, GT, GEQ };

struct Predicate {
    std::string attrName;
    OpType op;
    Value val;
}; // base
