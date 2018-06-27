#include <API/API.h>
#include <Interpreter/AST.h>
#include <SQLError.h>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <stdexcept>

namespace Interpreter {
namespace AST {

void CreateTableStatement::setTableName(const std::string &name) {
    tableName = name;
}

void CreateTableStatement::addAttribute(const Attribute &attr) {
    if (attributes.size() < 32) {
        attributes.push_back(attr);
    } else {
        throw SQLError("tables with more than 32 attributes are not supported");
    }
}

void CreateTableStatement::addPrimaryKey(const std::string &id) {
    if (primaryKey.empty()) {
        primaryKey = id;
    } else {
        throw SQLError("multiple primary keys are declared");
    }
}

void DropTableStatement::setTableName(const std::string &name) {
    tableName = name;
}

void CreateIndexStatement::setIndexName(const std::string &name) {
    indexName = name;
}

void CreateIndexStatement::setTableName(const std::string &name) {
    tableName = name;
}

void CreateIndexStatement::setAttrName(const std::string &name) {
    attrName = name;
}

void DropIndexStatement::setTableName(const std::string &name) {
    indexName = name;
}

void SelectStatement::setTableName(const std::string &name) {
    tableName = name;
}

void SelectStatement::addAttrName(const std::string &name) {
    attributes.push_back(name);
}

void SelectStatement::addPredicate(const Predicate &predicate) {
    predicates.push_back(predicate);
}

void InsertStatement::setTableName(const std::string &name) {
    tableName = name;
}

void InsertStatement::addValue(const Value &value) { values.push_back(value); }

void DeleteStatement::setTableName(const std::string &name) {
    tableName = name;
}

void DeleteStatement::addPredicate(const Predicate &predicate) {
    predicates.push_back(predicate);
}

void ExecfileStatement::setFilePath(const std::string &path) {
    filePath = path;
}

const std::string &ExecfileStatement::getFilePath() const { return filePath; }

void CreateTableStatement::callAPI() const {
    if (primaryKey.empty()) {
        throw SQLError("primary key not specified");
    }
    API::createTable(tableName, primaryKey, attributes);
    std::cout << "Table \'" << tableName << "\' has been created." << std::endl;
}

void DropTableStatement::callAPI() const {
    API::dropTable(tableName);
    std::cout << "Table \'" << tableName << "\' has been dropped." << std::endl;
}

void CreateIndexStatement::callAPI() const {
    API::createIndex(indexName, tableName, attrName);
    std::cout << "Index \'" << indexName << "\' has been created." << std::endl;
}

void DropIndexStatement::callAPI() const {
    API::dropIndex(indexName);
    std::cout << "Index \'" << indexName << "\' has been dropped." << std::endl;
}

void SelectStatement::callAPI() const {
    auto results = API::select(attributes, tableName, predicates);
    auto &schema = results.first;
    auto &records = results.second;
    if (records.empty()) {
        std::cout << "No records are selected." << std::endl;
    } else {
        auto attrNames(attributes);
        if (attrNames.empty()) {
            std::transform(schema->attributes.begin(), schema->attributes.end(),
                           std::back_inserter(attrNames),
                           [](const Attribute &attribute) -> std::string {
                               return attribute.name;
                           });
        }
        int num = attrNames.size();
        std::vector<size_t> widths(num, std::numeric_limits<size_t>::min());
        for (int i = 0; i < num; i++) {
            widths[i] = std::max(widths[i], attrNames[i].size());
        }
        for (auto &record : records) {
            for (int i = 0; i < num; i++) {
                widths[i] = std::max(widths[i], record[i].toString().size());
            }
        }
        auto outputEdge = [&]() {
            std::cout << "+";
            for (int i = 0; i < num; i++) {
                std::cout << std::string(widths[i] + 2, '-');
                std::cout << "+";
            }
            std::cout << std::endl;
        };
        outputEdge();
        std::cout << std::left;
        std::cout << "|";
        for (int i = 0; i < num; i++) {
            std::cout << " " << std::setw(widths[i]) << attrNames[i] << " |";
        }
        std::cout << std::endl;
        outputEdge();
        std::cout << std::right;
        for (auto &record : records) {
            int size = record.size();
            std::cout << "|";
            for (int i = 0; i < num; i++) {
                std::cout << " " << std::setw(widths[i]) << record[i].toString()
                          << " |";
            }
            std::cout << std::endl;
        }
        outputEdge();
    }
}

void InsertStatement::callAPI() const {
    API::insert(tableName, values);
    std::cout << "The new record has been inserted into table \'" << tableName
              << "\'." << std::endl;
}

void DeleteStatement::callAPI() const {
    int num = API::deleteFrom(tableName, predicates);
    if (num < 0) {
        throw std::logic_error("number of records cannot be negative");
    } else if (num == 0) {
        std::cout << "No records are deleted." << std::endl;
    } else if (num == 1) {
        std::cout << "1 record has been deleted." << std::endl;
    } else {
        std::cout << "A total of " << num;
        std::cout << " records have been deleted." << std::endl;
    }
}

void QuitStatement::callAPI() const {
    throw std::logic_error("no API for 'quit'");
}

void ExecfileStatement::callAPI() const {
    throw std::logic_error("no API for 'execfile'");
}

} // namespace AST
} // namespace Interpreter