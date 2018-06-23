#pragma once

#include <Interpreter/AST.h>
#include <Interpreter/Lexer.h>
#include <Interpreter/ParseError.h>
#include <Interpreter/Token.h>

namespace Interpreter {

using Statements = std::vector<std::shared_ptr<AST::Statement>>;

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
    std::pair<AttrType, size_t> getAttrType();
    void getTableDefns(std::shared_ptr<AST::CreateTableStatement>);
    int getInteger();
    float getFloating();
    std::string getString();
    void raise(const std::string &);

  public:
    Parser(std::istream &is);
    Statements parse();
    Statements parseCreate();
    Statements parseCreateTable();
};

} // namespace Interpreter