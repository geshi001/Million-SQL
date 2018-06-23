#include <Interpreter/REPL.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace Interpreter {

void REPL::run() {
    running = true;
    while (running) {
        std::cout << "MillionSQL> ";
        if (!std::getline(std::cin, line)) {
            running = false;
        }
        std::istringstream iss(line + "\n");
        try {
            Parser parser(iss);
            auto stmts = parser.parse();
            for (auto pStmt : stmts) {
                pStmt->callAPI();
            }
        } catch (std::exception &e) {
            std::cout << e.what() << std::endl;
        }
    }
}

} // namespace Interpreter