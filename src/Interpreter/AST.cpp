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
    primaryKeys.push_back(id);
}

void CreateTableStatement::check() const {
    if (primaryKeys.size() > 1) {
        throw std::runtime_error("SQLError: multiple primary keys");
    }
    if (primaryKeys.size() == 0) {
        throw std::runtime_error("SQLError: primary key not specified");
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

void CreateTableStatement::callAPI() const { throw "NotImplementedError"; }

void DropTableStatement::callAPI() const { throw "NotImplementedError"; }

void CreateIndexStatement::callAPI() const { throw "NotImplementedError"; }

void DropIndexStatement::callAPI() const { throw "NotImplementedError"; }

} // namespace AST
} // namespace Interpreter