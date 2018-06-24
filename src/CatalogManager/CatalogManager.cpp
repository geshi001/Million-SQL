#include <BufferManager/Block.h>
#include <CatalogManager/CatalogManager.h>
#include <DataType.h>
#include <map>
#include <stdexcept>
#include <string>

#include <iostream>

namespace CatalogManager {

std::map<std::string, Table> tables;

void init() {
    auto filename = File::catalogFilename();
    if (!BM::fileExists(filename)) {
        BM::createFile(filename, File::FileType::CATALOG);
    }
    BM::PtrBlock blk0 = BM::readBlock(BM::makeID(filename, 0));
}

} // namespace CatalogManager