#pragma once
#include <sstream>
#include <stdexcept>

class SQLError : public std::runtime_error {
  public:
    explicit SQLError(const std::string &what_arg)
        : std::runtime_error("\033[31mSQLError: \033[0m" + what_arg) {}
};