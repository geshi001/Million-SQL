#pragma once
#include <Error.h>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>

enum class ValueType { INT, FLOAT, CHAR };

struct Attribute {
    ValueType type;
    size_t charCnt;
    bool isUnique;
    std::string name;

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
};

struct Value {
    ValueType type;
    size_t charCnt;
    union {
        int ival;
        float fval;
        char cval[256];
    };

    Value() = default;
    Value(const Attribute &attribute)
        : type(attribute.type), charCnt(attribute.charCnt) {}

    const char *val() const {
        switch (type) {
        case ValueType::INT:
            return reinterpret_cast<const char *>(&ival);
        case ValueType::FLOAT:
            return reinterpret_cast<const char *>(&fval);
        case ValueType::CHAR:
            return cval;
        }
    }

    char *val() {
        switch (type) {
        case ValueType::INT:
            return reinterpret_cast<char *>(&ival);
        case ValueType::FLOAT:
            return reinterpret_cast<char *>(&fval);
        case ValueType::CHAR:
            return cval;
        }
    }

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
        case ValueType::FLOAT: {
            std::stringstream ss;
            ss << fval;
            auto str = ss.str();
            if (str.find_first_of('.') == std::string::npos)
                str += ".0";
            return str;
        }
        case ValueType::CHAR:
            return "\'" + std::string(cval) + "\'";
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

    bool operator==(const Value &rhs) const {
        if (type != rhs.type) {
            throw SQLError("cannot compare values with different types");
        }
        switch (type) {
        case ValueType::INT:
            return ival == rhs.ival;
        case ValueType::FLOAT:
            return fval == rhs.fval;
        case ValueType::CHAR:
            return std::strcmp(cval, rhs.cval) == 0;
        }
    }

    bool operator!=(const Value &rhs) const { return !(*this == rhs); }
    bool operator>(const Value &rhs) const { return rhs < *this; }
    bool operator<=(const Value &rhs) const { return !(*this > rhs); }
    bool operator>=(const Value &rhs) const { return !(*this < rhs); }
};

enum class OpType { EQ, NE, LT, LEQ, GT, GEQ };

struct Predicate {
    std::string attrName;
    OpType op;
    Value val;
};

struct Schema {
    std::string tableName;
    std::string primaryKey;
    std::vector<Attribute> attributes;
};

using Record = std::vector<Value>;

struct Index {
    std::string indexName;
    std::string tableName;
    std::string attrName;
};