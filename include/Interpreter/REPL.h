#pragma once
#include <Interpreter/Parser.h>
#include <Interpreter/REPL.h>

namespace Interpreter {

class REPL final {
  private:
    REPL() = default;
    ~REPL() = default;
    std::string line;
    std::stringstream ss;
    bool running;

  public:
    static REPL &repl() {
        static REPL instance;
        return instance;
    }
    void run();
};

} // namespace Interpreter