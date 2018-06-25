#include <FileSpec.h>
#include <RecordManager/RecordManager.h>

namespace RecordManager {

namespace BM = BufferManager;

void init() {}

void createTable(const std::string &tableName) {
    auto filename = File::tableFilename(tableName);
    if (!BM::fileExists(filename)) {
        BM::createFile(filename, File::FileType::TABLE);
    }
    BM::PtrBlock blk0 = BM::readBlock(BM::makeID(filename, 0));
}

void loadTable(const std::string &tableName) {
    auto filename = File::tableFilename(tableName);
}

void exit() {}

} // namespace RecordManager