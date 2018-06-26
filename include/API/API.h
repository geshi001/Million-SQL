#pragma once
#include <DataType.h>

class API {
  public:
    static void createTable(const std::string &tableName,
                            const std::string &primaryKey,
                            const std::vector<Attribute> &attributes);

    static void dropTable(const std::string &tableName);

    static void createIndex(const std::string &indexName,
                            const std::string &tableName,
                            const std::string &attrName);

    static void dropIndex(const std::string &indexName);

    static std::vector<Record>
    select(const std::vector<std::string> &attributes,
           const std::string &tableName,
           const std::vector<Predicate> &predicates);

    static void insert(const std::string &tableName,
                       const std::vector<Value> &values);

    static void deleteFrom(const std::string &tableName,
                           const std::vector<Predicate> &predicates);
};