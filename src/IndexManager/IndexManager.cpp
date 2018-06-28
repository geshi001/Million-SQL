#include <BufferManager/BufferManager.h>
#include <CatalogManager/CatalogManager.h>
#include <Error.h>
#include <FileSpec.h>
#include <IndexManager/IndexManager.h>

namespace IM {

void init() {
    auto &indices = CM::mapIndices;
    for (auto &index : indices) {
        if (!hasIndex(index.first)) {
            throw SysError("missing data for index \'" + index.first + "\'");
        }
    }
}

bool hasIndex(const std::string &indexName) {
    return BM::fileExists(File::indexFilename(indexName));
}

void createIndex(const std::string &indexName, const std::string &tableName,
                 const std::string &attrName) {
    if (!hasIndex(indexName)) {
        BM::createFile(File::indexFilename(indexName), File::FileType::INDEX);
    } else {
        throw Warning("file for index \'" + indexName + "\' already exists");
    }
}

void dropIndex(const std::string &indexName) {
    if (hasIndex(indexName)) {
        BM::deleteFile(File::indexFilename(indexName));
    } else {
        throw SysError("missing data for index \'" + indexName + "\'");
    }
}

void exit() {
    //
}

} // namespace IM
