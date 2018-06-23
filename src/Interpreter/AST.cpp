#include <Interpreter/AST.h>

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
        throw "SQLError: multiple primary keys";
    }
    if (primaryKeys.size() == 0) {
        throw "SQLError: primary key not specified";
    }
}

void CreateTableStatement::callAPI() const { throw "NotImplementedError"; }

void DropTableStatement::callAPI() const { throw "NotImplementedError"; }

} // namespace AST
} // namespace Interpreter