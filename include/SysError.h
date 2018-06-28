#pragma once
#include <sstream>
#include <stdexcept>

class SysError : public std::runtime_error {
  public:
    explicit SysError(const std::string &what_arg)
        : std::runtime_error("\033[31mSysError: \033[0m" + what_arg) {}
};