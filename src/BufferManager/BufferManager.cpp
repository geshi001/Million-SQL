#include <BufferManager/BufferManager.h>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <sys/stat.h>

namespace BufferManager {

static std::list<PtrBlock> cache;
static const char empty_buffer[BLOCK_SIZE] = {};

void init() { cache.clear(); }

void exit() {
    for (auto blkPtr : cache) {
        if (blkPtr->isDirty()) {
            blkPtr->writeFile();
            blkPtr->setDirty(false);
        }
    }
}

bool fileExists(const std::string &filename) {
    static struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

static void popCache() {
    while (cache.size() >= CACHE_SIZE) {
        for (auto iter = cache.rbegin(); iter != cache.rend(); ++iter) {
            auto blkPtr = *iter;
            if (blkPtr->isPinned()) {
                continue;
            } else if (blkPtr->isDirty()) {
                blkPtr->writeFile();
                blkPtr->setDirty(false);
            }
            cache.erase(std::next(iter).base()); // erase(iter)
            break;
        }
    }
}

void createFile(const std::string &filename, const File::FileType filetype) {
    popCache();
    auto blkPtr = std::make_shared<Block>(makeID(filename, 0));
    blkPtr->setFree(false);
    blkPtr->setDirty(true);
    std::memset(blkPtr->block_data, 0, BLOCK_SIZE);
    char *pos = blkPtr->block_data;
    auto write = [&pos](const char *src, size_t size) {
        std::memcpy(pos, src, size);
        pos += size;
    };
    switch (filetype) {
    case File::FileType::CATALOG: {
        File::catalogFileHeader header;
        header.filetype = static_cast<uint32_t>(filetype);
        header.blockNum = 1;
        header.tableOffset = 0;
        header.indexOffset = 0;
        write(reinterpret_cast<const char *>(&header), sizeof(header));
        write(empty_buffer, BLOCK_SIZE - sizeof(header));
        break;
    }
    case File::FileType::TABLE: {
        File::tableFileHeader header;
        header.filetype = static_cast<uint32_t>(filetype);
        header.blockNum = 1;
        header.beginOffset = 0;
        header.deletedOffset = 0;
        header.availableOffset = BLOCK_SIZE;
        write(reinterpret_cast<const char *>(&header), sizeof(header));
        write(empty_buffer, BLOCK_SIZE - sizeof(header));
        break;
    }
    }
    cache.push_front(blkPtr);
    blkPtr->createFile();
    blkPtr->setDirty(false);
}

void deleteFile(const std::string &filename) { std::remove(filename.c_str()); }

PtrBlock readBlock(const BlockID &id) {
    for (auto iter = cache.begin(); iter != cache.end(); ++iter) {
        auto blkPtr = *iter;
        if (blkPtr->getFilename() == id.first &&
            blkPtr->getOffset() == id.second) {
            cache.erase(iter);
            cache.push_front(blkPtr);
            return blkPtr;
        }
    }
    popCache();
    auto blkPtr = std::make_shared<Block>(id);
    blkPtr->readFile();
    blkPtr->setFree(false);
    cache.push_front(blkPtr);
    return blkPtr;
}

void writeBlock(const BlockID &id, const char *src, uint32_t start,
                size_t size) {
    for (auto blkPtr : cache) {
        if (blkPtr->getFilename() == id.first &&
            blkPtr->getOffset() == id.second) {
            blkPtr->setDirty(true);
            std::memcpy(blkPtr->block_data + start, src, size);
            return;
        }
    }
    auto blkPtr = readBlock(id);
    blkPtr->setDirty(true);
    std::memcpy(blkPtr->block_data + start, src, size);
    return;
}

} // namespace BufferManager