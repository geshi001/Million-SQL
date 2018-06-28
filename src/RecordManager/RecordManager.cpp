#include <CatalogManager/CatalogManager.h>
#include <Error.h>
#include <FileSpec.h>
#include <RecordManager/RecordManager.h>
#include <RecordManager/RecordSpec.h>
#include <algorithm>

namespace RM {

void init() {
    auto &schemas = CM::mapSchemas;
    for (auto &schema : schemas) {
        if (!hasTable(schema.first)) {
            throw SysError("missing data for table \'" + schema.first + "\'");
        }
    }
}

static bool satisfy(std::shared_ptr<Schema> schema, const Predicate &predicate,
                    const Record &record) {
    for (int i = 0; i < record.size(); i++) {
        auto &attribute = schema->attributes[i];
        if (predicate.attrName == attribute.name) {
            auto &value = record[i];
            switch (predicate.op) {
            case OpType::EQ:
                return value == predicate.val;
            case OpType::NE:
                return value != predicate.val;
            case OpType::LT:
                return value < predicate.val;
            case OpType::LEQ:
                return value <= predicate.val;
            case OpType::GT:
                return value > predicate.val;
            case OpType::GEQ:
                return value >= predicate.val;
            }
        }
    }
    throw SQLError("cannot find attribute \'" + predicate.attrName +
                   "\' in table \'" + schema->tableName + "\'");
}

bool hasTable(const std::string &tableName) {
    return BM::fileExists(File::tableFilename(tableName));
}

void createTable(const std::string &tableName) {
    if (!hasTable(tableName)) {
        BM::createFile(File::tableFilename(tableName), File::FileType::TABLE);
    } else {
        throw Warning("file for table \'" + tableName + "\' already exists");
    }
}

void dropTable(const std::string &tableName) {
    if (hasTable(tableName)) {
        BM::deleteFile(File::tableFilename(tableName));
    } else {
        throw SysError("missing data for table \'" + tableName + "\'");
    }
}

void insertRecord(const std::string &tableName, const Record &record) {
    auto filename = File::tableFilename(tableName);
    if (!hasTable(tableName)) {
        throw SysError("missing data for table \'" + tableName + "\'");
    }
    auto schema = CM::getSchema(tableName);
    BM::PtrBlock blk0 = BM::readBlock(BM::makeID(filename, 0));
    blk0->resetPos();
    File::tableFileHeader header;
    blk0->read(reinterpret_cast<char *>(&header), sizeof(header));
    if (header.filetype != static_cast<uint32_t>(File::FileType::TABLE)) {
        throw SysError("file type not compatible");
    }

    uint32_t newPos = header.availableOffset;
    uint32_t blkOff = BM::blockOffset(newPos);
    uint32_t inBlkOff = BM::inBlockOffset(newPos);
    BM::PtrBlock blk = BM::readBlock(BM::makeID(filename, blkOff));
    blk->resetPos(inBlkOff);

    uint32_t _offset = inBlkOff;
    auto write = [&](const char *src, size_t size) {
        BM::writeBlock(BM::makeID(filename, blkOff), src, _offset, size);
        _offset += size;
    };
    write(reinterpret_cast<const char *>(&header.beginOffset),
          sizeof(uint32_t));
    if (record.size() != schema->attributes.size()) {
        throw SQLError("value size mismatch");
    }
    for (int i = 0; i < record.size(); i++) {
        if (record[i].type != schema->attributes[i].type) {
            throw SQLError("value type mismatch");
        }
        if (record[i].type == ValueType::CHAR &&
            record[i].size() > schema->attributes[i].size()) {
            throw SQLError("string " + record[i].toString() +
                           " is too long to fit in char(" +
                           std::to_string(schema->attributes[i].size()) + ")");
        }
        write(record[i].val(), schema->attributes[i].size());
    }
    header.beginOffset = newPos;
    header.numRecords++;
    uint32_t size = recordBinarySize(*schema);
    if (inBlkOff + size >= BM::BLOCK_SIZE) {
        header.availableOffset = header.numBlocks++ * BM::BLOCK_SIZE;
    } else {
        header.availableOffset = newPos + size;
    }
    BM::writeBlock(BM::makeID(filename, 0),
                   reinterpret_cast<const char *>(&header), 0, sizeof(header));
}

int deleteAllRecords(const std::string &tableName) {
    auto filename = File::tableFilename(tableName);
    if (!hasTable(tableName)) {
        throw SysError("missing data for table \'" + tableName + "\'");
    }
    BM::PtrBlock blk0 = BM::readBlock(BM::makeID(filename, 0));
    blk0->resetPos();
    File::tableFileHeader header;
    blk0->read(reinterpret_cast<char *>(&header), sizeof(header));
    if (header.filetype != static_cast<uint32_t>(File::FileType::TABLE)) {
        throw SysError("file type not compatible");
    }
    int numDeleted = static_cast<int>(header.numRecords);
    header.numBlocks = 1;
    header.beginOffset = 0;
    header.availableOffset = BM::BLOCK_SIZE;
    header.numRecords = 0;
    BM::writeBlock(BM::makeID(filename, 0),
                   reinterpret_cast<const char *>(&header), 0, sizeof(header));
    return numDeleted;
}

int deleteRecords(const std::string &tableName,
                  const std::vector<uint32_t> &offsets) {
    auto filename = File::tableFilename(tableName);
    if (!hasTable(tableName)) {
        throw SysError("missing data for table \'" + tableName + "\'");
    }
    BM::PtrBlock blk0 = BM::readBlock(BM::makeID(filename, 0));
    blk0->resetPos();
    File::tableFileHeader header;
    blk0->read(reinterpret_cast<char *>(&header), sizeof(header));
    if (header.filetype != static_cast<uint32_t>(File::FileType::TABLE)) {
        throw SysError("file type not compatible");
    }
    for (uint32_t pos : offsets) {
        uint32_t blkOff = BM::blockOffset(pos);
        uint32_t inBlkOff = BM::inBlockOffset(pos);
        BM::PtrBlock blk = BM::readBlock(BM::makeID(filename, blkOff));
        blk->resetPos(inBlkOff);
        uint32_t mark;
        blk->read(reinterpret_cast<char *>(&mark), sizeof(uint32_t));
        if (mark & DELETED_MARK) {
            throw SysError("record has already been deleted");
        }
        mark |= DELETED_MARK;
        BM::writeBlock(BM::makeID(filename, blkOff),
                       reinterpret_cast<const char *>(&mark), inBlkOff,
                       sizeof(uint32_t));
    }
    header.numRecords -= offsets.size();
    BM::writeBlock(BM::makeID(filename, 0),
                   reinterpret_cast<const char *>(&header), 0, sizeof(header));
    return offsets.size();
}

int deleteRecords(std::shared_ptr<Schema> schema,
                  const std::vector<Predicate> &predicates) {
    auto &tableName = schema->tableName;
    auto filename = File::tableFilename(tableName);
    if (!hasTable(tableName)) {
        throw SysError("missing data for table \'" + tableName + "\'");
    }
    BM::PtrBlock blk0 = BM::readBlock(BM::makeID(filename, 0));
    blk0->resetPos();
    File::tableFileHeader header;
    blk0->read(reinterpret_cast<char *>(&header), sizeof(header));
    if (header.filetype != static_cast<uint32_t>(File::FileType::TABLE)) {
        throw SysError("file type not compatible");
    }
    int numDeleted = 0;
    uint32_t pos = header.beginOffset;
    while (pos != 0) {
        uint32_t blkOff = BM::blockOffset(pos);
        uint32_t inBlkOff = BM::inBlockOffset(pos);
        BM::PtrBlock blk = BM::readBlock(BM::makeID(filename, blkOff));
        blk->resetPos(inBlkOff);
        Record record;
        blk->read(reinterpret_cast<char *>(&pos), sizeof(uint32_t));
        if (pos & DELETED_MARK) {
            pos &= DELETED_MASK;
            continue;
        }
        for (auto &attribute : schema->attributes) {
            Value value(attribute);
            if (value.type == ValueType::CHAR) {
                std::memset(value.val(), 0, 256);
            }
            blk->read(value.val(), value.size());
            record.emplace_back(std::move(value));
        }
        bool chosen = true;
        for (auto predicate : predicates) {
            chosen &= satisfy(schema, predicate, record);
            if (!chosen) {
                break;
            }
        }
        if (chosen) {
            numDeleted++;
            uint32_t marked = pos | DELETED_MARK;
            BM::writeBlock(BM::makeID(filename, blkOff),
                           reinterpret_cast<const char *>(&marked), inBlkOff,
                           sizeof(uint32_t));
        }
    }
    header.numRecords -= numDeleted;
    BM::writeBlock(BM::makeID(filename, 0),
                   reinterpret_cast<const char *>(&header), 0, sizeof(header));
    return numDeleted;
}

std::vector<Record> selectRecords(std::shared_ptr<Schema> schema,
                                  const std::vector<Predicate> &predicates) {
    auto &tableName = schema->tableName;
    std::vector<Record> records;
    auto filename = File::tableFilename(tableName);
    if (!hasTable(tableName)) {
        throw SysError("missing data for table \'" + tableName + "\'");
    }
    BM::PtrBlock blk0 = BM::readBlock(BM::makeID(filename, 0));
    blk0->resetPos();
    File::tableFileHeader header;
    blk0->read(reinterpret_cast<char *>(&header), sizeof(header));
    if (header.filetype != static_cast<uint32_t>(File::FileType::TABLE)) {
        throw SysError("file type not compatible");
    }
    uint32_t pos = header.beginOffset;
    while (pos != 0) {
        uint32_t blkOff = BM::blockOffset(pos);
        uint32_t inBlkOff = BM::inBlockOffset(pos);
        BM::PtrBlock blk = BM::readBlock(BM::makeID(filename, blkOff));
        blk->resetPos(inBlkOff);
        Record record;
        blk->read(reinterpret_cast<char *>(&pos), sizeof(uint32_t));
        if (pos & DELETED_MARK) {
            pos &= DELETED_MASK;
            continue;
        }
        for (auto &attribute : schema->attributes) {
            Value value(attribute);
            if (value.type == ValueType::CHAR) {
                std::memset(value.val(), 0, 256);
            }
            blk->read(value.val(), value.size());
            record.emplace_back(std::move(value));
        }
        bool chosen = true;
        for (auto predicate : predicates) {
            chosen &= satisfy(schema, predicate, record);
            if (!chosen) {
                break;
            }
        }
        if (chosen) {
            records.push_back(record);
        }
    }
    std::reverse(records.begin(), records.end());
    return records;
}

std::vector<Record> project(const std::vector<Record> &records,
                            std::shared_ptr<Schema> schema,
                            const std::vector<std::string> &attributes) {
    if (attributes.empty()) {
        return records;
    }
    std::vector<int> permutation;
    for (auto &attrName : attributes) {
        bool found = false;
        for (int i = 0; !found && i < schema->attributes.size(); i++) {
            if (attrName == schema->attributes[i].name) {
                permutation.push_back(i);
                found = true;
            }
        }
        if (!found) {
            throw SQLError("cannot find attribute \'" + attrName +
                           "\' in table \'" + schema->tableName + "\'");
        }
    }
    std::vector<Record> results;
    for (auto &record : records) {
        Record projected;
        for (int pos : permutation) {
            projected.emplace_back(record[pos]);
        }
        results.emplace_back(projected);
    }
    return results;
}

void exit() {}

} // namespace RM