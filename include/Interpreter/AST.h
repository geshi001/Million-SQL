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
    virtual void callAPI() const = 0;
};

class CreateTableStatement : public Statement {
  public:
    std::string tableName;
    std::string primaryKey;
    std::vector<Attribute> attributes;

  public:
    void setTableName(const std::string &);
    void addAttribute(const Attribute &);
    void addPrimaryKey(const std::string &);
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

class SelectStatement : public Statement {
  public:
    std::vector<std::string> attributes;
    std::string tableName;
    std::vector<Predicate> predicates;

  public:
    void setTableName(const std::string &);
    void addAttrName(const std::string &);
    void addPredicate(const Predicate &);
    void callAPI() const override;
};

class InsertStatement : public Statement {
  public:
    std::string tableName;
    std::vector<Value> values;

  public:
    void setTableName(const std::string &);
    void addValue(const Value &);
    void callAPI() const override;
};

class DeleteStatement : public Statement {
  public:
    std::string tableName;
    std::vector<Predicate> predicates;

  public:
    void setTableName(const std::string &);
    void addPredicate(const Predicate &);
    void callAPI() const override;
};

class QuitStatement : public Statement {
  public:
    void callAPI() const override;
};

class ExecfileStatement : public Statement {
  public:
    std::string filePath;

  public:
    void setFilePath(const std::string &);
    void callAPI() const override;
};

} // namespace AST

} // namespace Interpreter