#include <API/API.h>
#include <iostream>

void API::createTable(const std::string &tableName,
                      const std::string &primaryKey,
                      const std::vector<Attribute> &attributes) {
    std::cout << "create table " << tableName << " (" << std::endl;

    for (int i = 0; i < attributes.size(); i++) {
        auto &attr = attributes[i];
        std::cout << "    " << attr.attrName;
        switch (attr.attrType) {
        case AttrType::INT: {
            std::cout << " int";
            break;
        }
        case AttrType::FLOAT: {
            std::cout << " float";
            break;
        }
        case AttrType::CHAR: {
            std::cout << " char(" << attr.charCnt << ")";
            break;
        }
        }
        if (attr.isUnique) {
            std::cout << " unique";
        }

        std::cout << ",\n";
    }
    std::cout << "    primary key (" << primaryKey << ")";
    std::cout << "\n);\n";
    std::cout << std::endl;
}

void API::dropTable(const std::string &tableName) {
    std::cout << "drop table " << tableName << ";" << std::endl;
    std::cout << std::endl;
}

void API::createIndex(const std::string &indexName,
                      const std::string &tableName,
                      const std::string &attrName) {
    std::cout << "create index " << indexName << std::endl;
    std::cout << "on " << tableName << "(" << attrName << ");\n";
    std::cout << std::endl;
}

void API::dropIndex(const std::string &indexName) {
    std::cout << "drop index " << indexName << ";" << std::endl;
    std::cout << std::endl;
}

void API::select(const std::vector<std::string> &attributes,
                 const std::string &tableName,
                 const std::vector<Predicate> &predicates) {
    std::cout << "select ";
    if (attributes.empty()) {
        std::cout << "* ";
    } else {
        for (int i = 0; i < attributes.size(); i++) {
            if (i)
                std::cout << ", ";
            std::cout << attributes[i];
        }
    }
    std::cout << '\n';
    std::cout << "from " << tableName;
    if (!predicates.empty()) {
        std::cout << "\nwhere ";
        for (int i = 0; i < predicates.size(); i++) {
            if (i)
                std::cout << " and ";
            auto &pred = predicates[i];
            std::cout << pred.attrName;

            if (pred.op == OpType::EQ) {
                std::cout << " = ";
            } else if (pred.op == OpType::NE) {
                std::cout << " <> ";
            } else if (pred.op == OpType::LT) {
                std::cout << " < ";
            } else if (pred.op == OpType::LEQ) {
                std::cout << " <= ";
            } else if (pred.op == OpType::GT) {
                std::cout << " > ";
            } else if (pred.op == OpType::GEQ) {
                std::cout << " >= ";
            }

            if (pred.val.attrType == AttrType::INT) {
                std::cout << pred.val.ival;
            } else if (pred.val.attrType == AttrType::FLOAT) {
                std::cout << pred.val.fval;
            } else if (pred.val.attrType == AttrType::CHAR) {
                std::cout << "\'" << pred.val.cval << "\'";
            }
        }
    }
    std::cout << ";" << std::endl;
    std::cout << std::endl;
}

void API::insert(const std::string &tableName,
                 const std::vector<Value> &values) {
    std::cout << "insert into " << tableName << std::endl;
    std::cout << "values (";
    for (int i = 0; i < values.size(); i++) {
        if (i)
            std::cout << ", ";
        auto &value = values[i];
        if (value.attrType == AttrType::INT) {
            std::cout << value.ival;
        } else if (value.attrType == AttrType::FLOAT) {
            std::cout << value.fval;
        } else if (value.attrType == AttrType::CHAR) {
            std::cout << "\'" << value.cval << "\'";
        }
    }
    std::cout << ");\n";
    std::cout << std::endl;
}

void API::deleteFrom(const std::string &tableName,
                     const std::vector<Predicate> &predicates) {
    std::cout << "delete ";
    std::cout << "from " << tableName;
    if (!predicates.empty()) {
        std::cout << "\nwhere ";
        for (int i = 0; i < predicates.size(); i++) {
            if (i)
                std::cout << " and ";
            auto &pred = predicates[i];
            std::cout << pred.attrName;

            if (pred.op == OpType::EQ) {
                std::cout << " = ";
            } else if (pred.op == OpType::NE) {
                std::cout << " <> ";
            } else if (pred.op == OpType::LT) {
                std::cout << " < ";
            } else if (pred.op == OpType::LEQ) {
                std::cout << " <= ";
            } else if (pred.op == OpType::GT) {
                std::cout << " > ";
            } else if (pred.op == OpType::GEQ) {
                std::cout << " >= ";
            }

            if (pred.val.attrType == AttrType::INT) {
                std::cout << pred.val.ival;
            } else if (pred.val.attrType == AttrType::FLOAT) {
                std::cout << pred.val.fval;
            } else if (pred.val.attrType == AttrType::CHAR) {
                std::cout << "\'" << pred.val.cval << "\'";
            }
        }
    }
    std::cout << ";" << std::endl;
    std::cout << std::endl;
}

void API::quit() {
    std::cout << "quiting..." << std::endl;
    exit(0);
    std::cout << std::endl;
}
