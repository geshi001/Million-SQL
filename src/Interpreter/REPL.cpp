#include <Error.h>
#include <Interpreter/REPL.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace Interpreter {

const char *PROMPT_MAIN = "\033[32mMillionSQL> \033[0m";
const char *PROMPT_NEXT = "\033[32m         -> \033[0m";

void REPL::run() {
    running = true;
    while (running) {
        std::cout << PROMPT_MAIN;
        ss.clear();
        bool reading = true;
        while (reading) {
            if (!std::getline(std::cin, line)) {
                running = false;
            } else if (line.find_first_not_of(' ') == std::string::npos) {
                reading = false;
            } else {
                ss << line << std::endl;
                if (line.find(';') != std::string::npos)
                    reading = false;
                else
                    std::cout << PROMPT_NEXT;
            }
        }
        if (running == false) {
            break;
        }
        ss << std::endl;
        try {
            Parser parser(ss);
            auto stmts = parser.parse();
            for (auto pStmt : stmts) {
                auto quitStmt = dynamic_cast<AST::QuitStatement *>(pStmt.get());
                auto execStmt =
                    dynamic_cast<AST::ExecfileStatement *>(pStmt.get());
                if (execStmt) {
                    auto filePath = execStmt->getFilePath();
                    std::ifstream ifs(filePath);
                    if (ifs.fail()) {
                        throw SQLError("script \'" + filePath +
                                       "\' does not exist");
                    }
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
                } else if (quitStmt) {
                    running = false;
                } else { // neither 'quit' nor 'execfile'
                    pStmt->callAPI();
                }
            }
            if (!running) {
                break;
            }
        } catch (std::exception &e) {
            std::cout << e.what() << std::endl;
        }
    }
} // namespace Interpreter

} // namespace Interpreter