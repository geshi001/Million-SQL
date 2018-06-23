#include <Interpreter/AST.h>
#include <stdexcept>

namespace Interpreter {
namespace AST {

void CreateTableStatement::setTableName(const std::string &name) {
    tableName = name;
}

void CreateTableStatement::addAttribute(const Attribute &attr) {
    attributes.push_back(attr);
}

void CreateTableStatement::addPrimaryKey(const std::string &id) {
    if (primaryKey.empty()) {
        primaryKey = id;
    } else {
        throw std::runtime_error("SQLError: multiple primary keys");
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

void CreateTableStatement::callAPI() const {
    if (primaryKey.empty()) {
        throw std::runtime_error("SQLError: primary key not specified");
    }
    throw "NotImplementedError";
}

void DropTableStatement::callAPI() const { throw "NotImplementedError"; }

void CreateIndexStatement::callAPI() const { throw "NotImplementedError"; }

void DropIndexStatement::callAPI() const { throw "NotImplementedError"; }

void SelectStatement::callAPI() const { throw "NotImplementedError"; }

void InsertStatement::callAPI() const { throw "NotImplementedError"; }

void DeleteStatement::callAPI() const { throw "NotImplementedError"; }

void QuitStatement::callAPI() const { throw "NotImplementedError"; }

void ExecfileStatement::callAPI() const { throw "NotImplementedError"; }

} // namespace AST
} // namespace Interpreter