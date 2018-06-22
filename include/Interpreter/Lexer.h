#pragma once
#include <istream>
#include <string>

namespace Interpreter {

class Token {
  public:
    Token(const std::string &, const int nl, const int nc);
    std::string getText() const;
    int getNl() const;
    int getNc() const;

  private:
    std::string text;
    int nl, nc;
};

} // namespace Interpreter