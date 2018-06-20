#pragma once
#include <string>
#include <vector>

enum class AttrType { INT, FLOAT, CHAR };

struct Attribute {
    AttrType attrType;  // "char", "int", "float"
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
    } val;
};

enum class OpType { EQ, NE, LE, GE, LEQ, GEQ };
struct QueryCondition {
    std::string attrName;
    OpType op;
    Value val;
};  // base
