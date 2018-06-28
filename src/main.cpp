#include <BufferManager/BufferManager.h>
#include <CatalogManager/CatalogManager.h>
#include <IndexManager/IndexManager.h>
#include <Interpreter/REPL.h>
#include <RecordManager/RecordManager.h>
#include <exception>
#include <iostream>

using namespace std;

int main() {
    try {
        BM::init();
        CM::init();
        RM::init();
        IM::init();

        auto &repl = Interpreter::REPL::repl();
        repl.run();

        IM::exit();
        RM::exit();
        CM::exit();
        BM::exit();
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        std::cout << "MillionSQL initialization failed" << std::endl;
    }
    return 0;
}