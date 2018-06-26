#include <API/API.h>
#include <CatalogManager/CatalogManager.h>
#include <RecordManager/RecordManager.h>
#include <iostream>

namespace CM = CatalogManager;
namespace RM = RecordManager;

void API::createTable(const std::string &tableName,
                      const std::string &primaryKey,
                      const std::vector<Attribute> &attributes) {
    CM::createTable(tableName, primaryKey, attributes);
    RM::createTable(tableName);
}

void API::dropTable(const std::string &tableName) {
    CM::dropTable(tableName);
    RM::dropTable(tableName);
}

void API::createIndex(const std::string &indexName,
                      const std::string &tableName,
                      const std::string &attrName) {}

void API::dropIndex(const std::string &indexName) {}

std::vector<Record> API::select(const std::vector<std::string> &attributes,
                                const std::string &tableName,
                                const std::vector<Predicate> &predicates) {
    auto schema = CM::getSchema(tableName);
    auto records =
        RM::project(RM::selectRecords(schema, predicates), schema, attributes);
    return records;
}

void API::insert(const std::string &tableName,
                 const std::vector<Value> &values) {
    RM::insertRecord(tableName, values);
}

void API::deleteFrom(const std::string &tableName,
                     const std::vector<Predicate> &predicates) {
    if (predicates.empty()) {
        RM::deleteAllRecords(tableName);
    } else {
        auto schema = CM::getSchema(tableName);
        RM::deleteRecords(schema, predicates);
    }
}
