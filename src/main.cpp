#include <BufferManager/BufferManager.h>
#include <CatalogManager/CatalogManager.h>
#include <IndexManager/IndexManager.h>
#include <Interpreter/REPL.h>
#include <RecordManager/RecordManager.h>

using namespace std;

int main() {
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
    return 0;
}