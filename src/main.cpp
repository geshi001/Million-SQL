#include <BufferManager/BufferManager.h>
#include <CatalogManager/CatalogManager.h>
#include <Interpreter/REPL.h>
#include <RecordManager/RecordManager.h>

using namespace std;

int main() {
    BM::init();
    CM::init();
    RM::init();

    auto &repl = Interpreter::REPL::repl();
    repl.run();

    RM::exit();
    CM::exit();
    BM::exit();
    return 0;
}