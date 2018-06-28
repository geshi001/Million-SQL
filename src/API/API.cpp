#include <API/API.h>
#include <CatalogManager/CatalogManager.h>
#include <IndexManager/IndexManager.h>
#include <RecordManager/RecordManager.h>

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
                      const std::string &attrName) {
    CM::createIndex(indexName, tableName, attrName);
    IM::createIndex(indexName, tableName, attrName);
}

void API::dropIndex(const std::string &indexName) {
    CM::dropIndex(indexName);
    IM::dropIndex(indexName);
}

std::pair<std::shared_ptr<Schema>, std::vector<Record>>
API::select(const std::vector<std::string> &attributes,
            const std::string &tableName,
            const std::vector<Predicate> &predicates) {
    auto schema = CM::getSchema(tableName);
    auto records =
        RM::project(RM::selectRecords(schema, predicates), schema, attributes);
    return std::make_pair(schema, records);
}

void API::insert(const std::string &tableName,
                 const std::vector<Value> &values) {
    RM::insertRecord(tableName, values);
}

int API::deleteFrom(const std::string &tableName,
                    const std::vector<Predicate> &predicates) {
    if (predicates.empty()) {
        return RM::deleteAllRecords(tableName);
    } else {
        auto schema = CM::getSchema(tableName);
        return RM::deleteRecords(schema, predicates);
    }
}
