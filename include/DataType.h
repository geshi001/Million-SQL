#pragma once
#include <SQLError.h>
#include <cstring>
#include <string>
#include <vector>

enum class ValueType { INT, FLOAT, CHAR };

struct Attribute {
    ValueType type;
    size_t charCnt;
    bool isUnique;
    std::string name;
};

struct Value {
    ValueType type;
    size_t charCnt;
    union {
        int ival;
        float fval;
        char cval[256];
    };

    size_t size() const {
        switch (type) {
        case ValueType::INT:
            return sizeof(int);
        case ValueType::FLOAT:
            return sizeof(float);
        case ValueType::CHAR:
            return charCnt;
        }
    }

    std::string toString() const {
        switch (type) {
        case ValueType::INT:
            return std::to_string(ival);
        case ValueType::FLOAT:
            return std::to_string(fval);
        case ValueType::CHAR:
            return std::string(cval);
        }
    }

    bool operator<(const Value &rhs) const {
        if (type != rhs.type) {
            throw SQLError("cannot compare values with different types");
        }
        switch (type) {
        case ValueType::INT:
            return ival < rhs.ival;
        case ValueType::FLOAT:
            return fval < rhs.fval;
        case ValueType::CHAR:
            return std::strcmp(cval, rhs.cval) < 0;
        }
    }
};

enum class OpType { EQ, NE, LT, LEQ, GT, GEQ };

struct Predicate {
    std::string attrName;
    OpType op;
    Value val;
};

struct Table {
    std::string tableName;
    std::vector<Attribute> attributes;
};