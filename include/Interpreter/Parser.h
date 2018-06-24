#pragma once

#include <Interpreter/AST.h>
#include <Interpreter/Lexer.h>
#include <Interpreter/ParseError.h>
#include <Interpreter/Token.h>

namespace Interpreter {

using Statements = std::vector<std::shared_ptr<AST::Statement>>;
using PtrStmt = std::shared_ptr<AST::Statement>;

class Parser {
  private:
    Lexer lexer;
    std::vector<Token> tokens;
    std::vector<Token>::iterator p;
    bool check(const Keyword &);
    bool check(const Symbol &);
    void expect(const Keyword &);
    void expect(const Symbol &);
    std::string getIdentifier();
    std::pair<ValueType, size_t> getAttrType();
    void getTableDefns(std::shared_ptr<AST::CreateTableStatement>);
    Predicate getPredicate();
    Value getValue();
    int getInteger();
    float getFloating();
    std::string getString();
    void raise(const std::string &);
    inline void skip();

  public:
    Parser(std::istream &is);
    Statements parse();
    PtrStmt parseCreate();
    PtrStmt parseDrop();
    PtrStmt parseCreateTable();
    PtrStmt parseDropTable();
    PtrStmt parseCreateIndex();
    PtrStmt parseDropIndex();
    PtrStmt parseSelect();
    PtrStmt parseInsert();
    PtrStmt parseDelete();
    PtrStmt parseQuit();
    PtrStmt parseExecfile();
};

} // namespace Interpreter