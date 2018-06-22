#pragma once
#include <Interpreter/Token.h>
#include <cctype>
#include <istream>
#include <string>
#include <vector>

namespace Interpreter {

class Lexer {
  public:
    Lexer(std::istream &);
    std::vector<Token> tokenize();

  private:
    Token nextToken();
    bool nextLine();
    void skipChars(int);
    bool skipSpaces();

    std::istream &is;
    std::string currLine;
    std::string::iterator p;
    int nl, nc;
};

} // namespace Interpreter