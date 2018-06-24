#include <Interpreter/REPL.h>
#include <SQLError.h>
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
                auto execStmt =
                    dynamic_cast<AST::ExecfileStatement *>(pStmt.get());
                if (!execStmt) { // not 'execfile', invoke API
                    pStmt->callAPI();
                } else {
                    auto filePath = execStmt->getFilePath();
                    std::ifstream ifs(filePath);
                    Interpreter::Parser fileParser(ifs);
                    for (auto fileStmt : fileParser.parse()) {
                        auto quitStmt =
                            dynamic_cast<AST::QuitStatement *>(fileStmt.get());
                        auto execStmt = dynamic_cast<AST::ExecfileStatement *>(
                            fileStmt.get());
                        if (quitStmt) {
                            throw SQLError("'quit' is not supported in files");
                        } else if (execStmt) {
                            throw SQLError("'execfile' is "
                                           "not supported in files");
                        } else {
                            fileStmt->callAPI();
                        }
                    }
                }
            }
        } catch (std::exception &e) {
            std::cout << e.what() << std::endl;
        }
    }
}

} // namespace Interpreter