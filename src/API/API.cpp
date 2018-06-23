#include <API/API.h>
#include <iostream>

void API::createTable(const std::string &tableName,
                      const std::string &primaryKey,
                      const std::vector<Attribute> &attributes) {
    std::cout << "create table " << tableName << " (" << std::endl;
    for (auto &attr : attributes) {
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
    std::cout << ");\n";
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
        for (auto &name : attributes)
            std::cout << name << ", ";
    }
    std::cout << '\n';
    std::cout << "from " << tableName;
    if (!predicates.empty()) {
        std::cout << "\nwhere ";
        for (auto pred : predicates) {
            std::cout << pred.attrName << " and ";
        }
    }
    std::cout << ";" << std::endl;
    std::cout << std::endl;
}

void API::insert(const std::string &tableName,
                 const std::vector<Value> &values) {
    std::cout << "insert into " << tableName << std::endl;
    std::cout << std::endl;
}

void API::deleteFrom(const std::string &tableName,
                     const std::vector<Predicate> &predicates) {
    std::cout << "delete from " << tableName << std::endl;
    std::cout << std::endl;
}

void API::quit() {
    std::cout << "quit;" << std::endl;
    std::cout << std::endl;
}

void API::execfile(const std::string &filePath) {
    std::cout << "execfile [" << filePath << "];" << std::endl;
    std::cout << std::endl;
}