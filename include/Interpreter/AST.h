#pragma once
#include <DataType.h>
#include <Interpreter/Token.h>
#include <memory>
#include <vector>

namespace Interpreter {

namespace AST {

using Identifier = std::string;

using PredicateList = std::vector<QueryCondition>;

using ValueList = std::vector<Value>;

class Statement {
  public:
    Statement() = default;
    virtual ~Statement() = default;
    virtual void check() const {};
    virtual void callAPI() const = 0;
};

class CreateTableStatement : public Statement {
  public:
    std::string tableName;
    std::vector<Attribute> attributes;
    std::vector<std::string> primaryKeys;

  public:
    void setTableName(const std::string &);
    void addAttribute(const Attribute &);
    void addPrimaryKey(const std::string &);
    void check() const override;
    void callAPI() const override;
};

class DropTableStatement : public Statement {
  private:
    std::string tableName;
    void callAPI() const override;
};

} // namespace AST

} // namespace Interpreter