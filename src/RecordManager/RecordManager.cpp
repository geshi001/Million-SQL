#include <CatalogManager/CatalogManager.h>
#include <FileSpec.h>
#include <RecordManager/RecordManager.h>
#include <RecordManager/RecordSpec.h>

namespace RecordManager {

namespace BM = BufferManager;

void init() {}

void createTable(const std::string &tableName) {
    auto filename = File::tableFilename(tableName);
    if (!BM::fileExists(filename)) {
        BM::createFile(filename, File::FileType::TABLE);
    } else {
        throw SQLError("table \'" + tableName + "\' already exists");
    }
}

void insertRecord(const std::string &tableName, const Record &record) {
    auto filename = File::tableFilename(tableName);
    if (!BM::fileExists(filename)) {
        throw SQLError("table \'" + tableName + "\' not exist");
    }
    auto schema = CM::getSchema(tableName);
    BM::PtrBlock blk0 = BM::readBlock(BM::makeID(filename, 0));
    blk0->resetPos();
    File::tableFileHeader header;
    blk0->read(reinterpret_cast<char *>(&header), sizeof(header));
    if (header.filetype != static_cast<uint32_t>(File::FileType::TABLE)) {
        throw std::runtime_error("file type not compatible");
    }

    if (header.deletedOffset != 0) {
        uint32_t newPos = header.deletedOffset;
        uint32_t blkOff = BM::blockOffset(newPos);
        uint32_t inBlkOff = BM::inBlockOffset(newPos);
        BM::PtrBlock blk = BM::readBlock(BM::makeID(filename, blkOff));
        blk->resetPos(inBlkOff);
        uint32_t newDelOff;
        blk->read(reinterpret_cast<char *>(&newDelOff), sizeof(uint32_t));

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
                record[i].size() >= schema->attributes[i].size()) {
                throw SQLError("string too long");
            }
            write(record[i].val(), schema->attributes[i].size());
        }
        header.beginOffset = newPos;
        header.deletedOffset = newDelOff;
        BM::writeBlock(BM::makeID(filename, 0),
                       reinterpret_cast<const char *>(&header), 0,
                       sizeof(header));
        return;
    } else {
        // no space marked as deleted
        // allocate new space
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
                record[i].size() >= schema->attributes[i].size()) {
                throw SQLError("string too long");
            }
            write(record[i].val(), schema->attributes[i].size());
        }
        header.beginOffset = newPos;
        uint32_t size = recordBinarySize(record);
        if (inBlkOff + size >= BM::BLOCK_SIZE) {
            header.availableOffset = (blkOff + 1) * BM::BLOCK_SIZE;
        } else {
            header.availableOffset = newPos + size;
        }
        BM::writeBlock(BM::makeID(filename, 0),
                       reinterpret_cast<const char *>(&header), 0,
                       sizeof(header));
    }
}

std::vector<Record> loadTable(std::shared_ptr<Schema> schema) {
    auto &tableName = schema->tableName;
    std::vector<Record> records;
    auto filename = File::tableFilename(tableName);
    if (!BM::fileExists(filename)) {
        throw SQLError("table \'" + tableName + "\' not exist");
    }
    BM::PtrBlock blk0 = BM::readBlock(BM::makeID(filename, 0));
    blk0->resetPos();
    File::tableFileHeader header;
    blk0->read(reinterpret_cast<char *>(&header), sizeof(header));
    if (header.filetype != static_cast<uint32_t>(File::FileType::TABLE)) {
        throw std::runtime_error("file type not compatible");
    }
    uint32_t pos = header.beginOffset;
    while (pos != 0) {
        uint32_t blkOff = BM::blockOffset(pos);
        uint32_t inBlkOff = BM::inBlockOffset(pos);
        BM::PtrBlock blk = BM::readBlock(BM::makeID(filename, blkOff));
        blk->resetPos(inBlkOff);
        Record record;
        blk->read(reinterpret_cast<char *>(&pos), sizeof(uint32_t));
        for (auto &attribute : schema->attributes) {
            Value value(attribute);
            if (value.type == ValueType::CHAR) {
                std::memset(value.val(), 0, 256);
            }
            blk->read(value.val(), value.size());
            record.emplace_back(std::move(value));
        }
        records.push_back(record);
    }
    return records;
}

void exit() {}

} // namespace RecordManager