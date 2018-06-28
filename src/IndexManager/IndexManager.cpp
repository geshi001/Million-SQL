#include <CatalogManager/CatalogManager.h>
#include <IndexManager/IndexManager.h>
#include <SysError.h>

namespace IM {

void init() {
    auto &indices = CM::mapIndices;
    for (auto &index : indices) {
        if (!hasTable(index.first)) {
            throw SysError("missing data for index \'" + index.first + "\'");
        }
    }
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
