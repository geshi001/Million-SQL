#include <IndexManager/IndexManager.h>

namespace IM {

void init() {
    //
}

bool hasTable(const std::string &indexName) {
    return BM::fileExists(File::indexFilename(indexName));
}

void createIndex(const std::string &indexName, const std::string &tableName,
                 const std::string &attrName) {
    //
}

void dropIndex(const std::string &indexName) {
    //
}

void exit() {
    //
}

} // namespace IM
