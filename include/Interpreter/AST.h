#pragma once
#include <DataType.h>
#include <Interpreter/Token.h>
#include <memory>
#include <vector>

namespace Interpreter {

namespace AST {

using Identifier = std::string;

using PredicateList = std::vector<Predicate>;

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
  public:
    std::string tableName;

  public:
    void setTableName(const std::string &);
    void callAPI() const override;
};

class CreateIndexStatement : public Statement {
  public:
    std::string indexName;
    std::string tableName;
    std::string attrName;

  public:
    void setIndexName(const std::string &);
    void setTableName(const std::string &);
    void setAttrName(const std::string &);
    void callAPI() const override;
};

class DropIndexStatement : public Statement {
  public:
    std::string indexName;

  public:
    void setTableName(const std::string &);
    void callAPI() const override;
};

} // namespace AST

} // namespace Interpreter