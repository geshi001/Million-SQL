#include <BufferManager/Block.h>
#include <CatalogManager/CatalogManager.h>
#include <CatalogManager/TableSpec.h>
#include <algorithm>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>

namespace CM {

std::unordered_map<std::string, std::shared_ptr<Schema>> mapSchemas;
std::unordered_map<std::string, uint32_t> mapSchemaOffsets;

std::unordered_map<std::string, Index> mapIndices;
std::unordered_map<std::string, uint32_t> mapIndexOffsets;

bool hasTable(const std::string &tableName) {
    return mapSchemas.find(tableName) != mapSchemas.end();
}

bool hasIndex(const std::string &indexName) {
    return mapIndices.find(indexName) != mapIndices.end();
}

void init() {
    mapSchemas.clear();
    mapSchemaOffsets.clear();

    mapIndices.clear();
    mapIndexOffsets.clear();

    auto filename = File::catalogFilename();
    if (!BM::fileExists(filename)) {
        BM::createFile(filename, File::FileType::CATALOG);
    }
    BM::PtrBlock blk0 = BM::readBlock(BM::makeID(filename, 0));
    blk0->resetPos();
    File::catalogFileHeader header;
    blk0->read(reinterpret_cast<char *>(&header), sizeof(header));

    uint32_t currP = header.tableOffset, nextP = 0;
    static char strbuf[NAME_LENGTH];

    while (currP != 0) {
        BM::PtrBlock blk = BM::readBlock(BM::makeID(filename, currP));
        blk->resetPos();
        auto schema = std::make_shared<Schema>();
        uint32_t numAttrs = 0;
        blk->read(reinterpret_cast<char *>(&nextP), sizeof(uint32_t));
        if (nextP & DELETED_MARK) {
            currP = nextP & DELETED_MASK;
            continue;
        }
        blk->read(reinterpret_cast<char *>(&numAttrs), sizeof(uint32_t));
        blk->read(strbuf, NAME_LENGTH);
        schema->tableName = std::string(strbuf);
        blk->read(strbuf, NAME_LENGTH);
        schema->primaryKey = std::string(strbuf);
        for (auto i = 0u; i != numAttrs; ++i) {
            Attribute attribute;
            blk->read(strbuf, NAME_LENGTH);
            attribute.name = std::string(strbuf);
            uint32_t bin;
            blk->read(reinterpret_cast<char *>(&bin), sizeof(uint32_t));
            std::tie(attribute.type, attribute.charCnt, attribute.isUnique) =
                decodeProperties(bin);
            schema->attributes.push_back(attribute);
        }
        mapSchemas[schema->tableName] = schema;
        mapSchemaOffsets[schema->tableName] = currP;
        currP = nextP;
    }

    currP = header.indexOffset;
    nextP = 0;
    while (currP != 0) {
        BM::PtrBlock blk = BM::readBlock(BM::makeID(filename, currP));
        blk->resetPos();
        blk->read(reinterpret_cast<char *>(&nextP), sizeof(uint32_t));
        if (nextP & DELETED_MARK) {
            currP = nextP & DELETED_MASK;
            continue;
        }
        Index index;
        blk->read(strbuf, NAME_LENGTH);
        index.indexName = std::string(strbuf);
        blk->read(strbuf, NAME_LENGTH);
        index.tableName = std::string(strbuf);
        blk->read(strbuf, NAME_LENGTH);
        index.attrName = std::string(strbuf);
        mapIndices[index.indexName] = index;
        mapIndexOffsets[index.indexName] = currP;
        currP = nextP;
    }
}

void createTable(const std::string &tableName, const std::string &primaryKey,
                 const std::vector<Attribute> &_attributes) {
    if (hasTable(tableName)) {
        throw SQLError("table \'" + tableName + "\' already exists");
    }
    std::vector<Attribute> attributes(_attributes);
    bool foundPrimaryKey = false;
    for (int i = 0; i < attributes.size(); i++) {
        if (attributes[i].name == primaryKey) {
            attributes[i].isUnique = true;
            foundPrimaryKey = true;
            break;
        }
    }
    if (!foundPrimaryKey) {
        throw SQLError("cannot find attribute \'" + primaryKey +
                       "\' in table \'" + tableName + "\'");
    }
    auto schema = std::make_shared<Schema>();
    schema->tableName = tableName;
    schema->primaryKey = primaryKey;
    schema->attributes = attributes;
    mapSchemas[tableName] = schema;

    auto filename = File::catalogFilename();
    BM::PtrBlock blk0 = BM::readBlock(BM::makeID(filename, 0));
    File::catalogFileHeader header;
    blk0->resetPos();
    blk0->read(reinterpret_cast<char *>(&header), sizeof(header));

    uint32_t newP = header.numBlocks++;
    uint32_t nextP = header.tableOffset;
    uint32_t numAttrs = attributes.size();
    header.tableOffset = newP;
    mapSchemaOffsets[tableName] = newP;

    uint32_t offset0 = 0, offsetP = 0;
    auto write0 = [&](const char *src, size_t size) {
        BM::writeBlock(BM::makeID(filename, 0), src, offset0, size);
        offset0 += size;
    };
    auto writeP = [&](const char *src, size_t size) {
        BM::writeBlock(BM::makeID(filename, newP), src, offsetP, size);
        offsetP += size;
    };
    write0(reinterpret_cast<const char *>(&header), sizeof(header));

    writeP(reinterpret_cast<const char *>(&nextP), sizeof(uint32_t));
    writeP(reinterpret_cast<const char *>(&numAttrs), sizeof(uint32_t));
    static char strbuf[NAME_LENGTH];

    memset(strbuf, 0, NAME_LENGTH);
    std::strcpy(strbuf, tableName.c_str());
    writeP(strbuf, NAME_LENGTH);

    memset(strbuf, 0, NAME_LENGTH);
    std::strcpy(strbuf, primaryKey.c_str());
    writeP(strbuf, NAME_LENGTH);

    for (auto &attribute : attributes) {
        uint32_t bin = encodeProperties(attribute);
        memset(strbuf, 0, NAME_LENGTH);
        std::strcpy(strbuf, attribute.name.c_str());
        writeP(strbuf, NAME_LENGTH);
        writeP(reinterpret_cast<const char *>(&bin), sizeof(uint32_t));
    }
}

void dropTable(const std::string &tableName) {
    if (!hasTable(tableName)) {
        throw SQLError("table \'" + tableName + "\' does not exist");
    }
    auto filename = File::catalogFilename();
    uint32_t offset = mapSchemaOffsets[tableName], nextP;

    BM::PtrBlock blk = BM::readBlock(BM::makeID(filename, offset));
    blk->resetPos(0);
    blk->read(reinterpret_cast<char *>(&nextP), sizeof(uint32_t));
    nextP |= DELETED_MARK;
    BM::writeBlock(BM::makeID(filename, offset),
                   reinterpret_cast<const char *>(&nextP), 0, sizeof(uint32_t));
    mapSchemas.erase(tableName);
    mapSchemaOffsets.erase(tableName);
}

void createIndex(const std::string &indexName, const std::string &tableName,
                 const std::string &attrName) {
    if (hasIndex(indexName)) {
        throw SQLError("index \'" + indexName + "\' already exists");
    }
    if (!hasTable(tableName)) {
        throw SQLError("table \'" + tableName + "\' does not exist");
    }
    auto schema = mapSchemas[tableName];
    auto &attributes = schema->attributes;
    if (std::none_of(attributes.begin(), attributes.end(),
                     [&](const Attribute &attribute) -> bool {
                         return attribute.name == attrName;
                     })) {
        throw SQLError("cannot find attribute \'" + attrName +
                       "\' in table \'" + tableName + "\'");
    }

    Index index;
    index.indexName = indexName;
    index.tableName = tableName;
    index.attrName = attrName;
    mapIndices[indexName] = index;

    auto filename = File::catalogFilename();
    BM::PtrBlock blk0 = BM::readBlock(BM::makeID(filename, 0));
    File::catalogFileHeader header;
    blk0->resetPos();
    blk0->read(reinterpret_cast<char *>(&header), sizeof(header));

    uint32_t newP = header.numBlocks++;
    uint32_t nextP = header.indexOffset;
    header.indexOffset = newP;
    mapIndexOffsets[indexName] = newP;

    uint32_t offset0 = 0, offsetP = 0;
    auto write0 = [&](const char *src, size_t size) {
        BM::writeBlock(BM::makeID(filename, 0), src, offset0, size);
        offset0 += size;
    };
    auto writeP = [&](const char *src, size_t size) {
        BM::writeBlock(BM::makeID(filename, newP), src, offsetP, size);
        offsetP += size;
    };

    write0(reinterpret_cast<const char *>(&header), sizeof(header));
    writeP(reinterpret_cast<const char *>(&nextP), sizeof(uint32_t));
    static char strbuf[NAME_LENGTH];

    memset(strbuf, 0, NAME_LENGTH);
    std::strcpy(strbuf, indexName.c_str());
    writeP(strbuf, NAME_LENGTH);

    memset(strbuf, 0, NAME_LENGTH);
    std::strcpy(strbuf, tableName.c_str());
    writeP(strbuf, NAME_LENGTH);

    memset(strbuf, 0, NAME_LENGTH);
    std::strcpy(strbuf, attrName.c_str());
    writeP(strbuf, NAME_LENGTH);
}

void dropIndex(const std::string &indexName) {
    if (!hasIndex(indexName)) {
        throw SQLError("index \'" + indexName + "\' does not exist");
    }
    auto filename = File::catalogFilename();
    uint32_t offset = mapIndexOffsets[indexName], nextP;

    BM::PtrBlock blk = BM::readBlock(BM::makeID(filename, offset));
    blk->resetPos(0);
    blk->read(reinterpret_cast<char *>(&nextP), sizeof(uint32_t));
    nextP |= DELETED_MARK;
    BM::writeBlock(BM::makeID(filename, offset),
                   reinterpret_cast<const char *>(&nextP), 0, sizeof(uint32_t));
    mapIndices.erase(indexName);
    mapIndexOffsets.erase(indexName);
}

void checkPredicates(const std::string &tableName,
                     const std::vector<Predicate> &predicates) {
    auto schema = getSchema(tableName);
    for (auto &predicate : predicates) {
        bool found = false;
        for (auto &attribute : schema->attributes) {
            if (attribute.name == predicate.attrName) {
                if (attribute.type == predicate.val.type) {
                    found = true;
                    break;
                } else {
                    throw SQLError(
                        "cannot compare values with different types");
                }
            }
        }
        if (!found) {
            throw SQLError("cannot find attribute \'" + predicate.attrName +
                           "\' in table \'" + schema->tableName + "\'");
        }
    }
}

void exit() {}

std::shared_ptr<Schema> getSchema(const std::string &tableName) {
    auto i = mapSchemas.find(tableName);
    if (i != mapSchemas.end()) {
        return i->second;
    } else {
        throw SQLError("cannot find table \'" + tableName + "\'");
    }
}

} // namespace CM