#pragma once
#include <stdexcept>
#include <string>

namespace Interpreter {

enum class end_of_file { eof };

class ParseError : public std::runtime_error {
  public:
    explicit ParseError(const std::string &, const int nl, const int nc);
    const char *what() const noexcept;

  private:
    int nl, nc;
};
} // namespace Interpreter