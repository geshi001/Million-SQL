#include <BufferManager/BufferManager.h>
#include <cstring>
#include <fstream>
#include <list>
#include <sys/stat.h>

namespace BufferManager {

static std::list<PtrBlock> cache;
static const char empty_buffer[BLOCK_SIZE] = {};

void init() { cache.clear(); }

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
                blkPtr->write();
                blkPtr->setDirty(false);
            }
            cache.erase(std::next(iter).base()); // erase(iter)
            break;
        }
    }
}

void createFile(const std::string &filename, const File::FileType filetype) {
    std::ofstream ofs;
    ofs.open(filename, std::ios::out | std::ios::binary);
    switch (filetype) {
    case File::FileType::CATALOG: {
        File::catalogFileHeader header;
        header.blockNum = 1;
        header.tableOffset = 0;
        ofs.write(reinterpret_cast<const char *>(&filetype), sizeof(uint32_t));
        ofs.write(reinterpret_cast<const char *>(&header), sizeof(header));
        ofs.write(empty_buffer, BLOCK_SIZE - sizeof(header) - sizeof(uint32_t));
        break;
    }
    }
    ofs.flush();
    ofs.close();
}

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
    blkPtr->read();
    blkPtr->setFree(false);
    cache.push_front(blkPtr);
    return blkPtr;
}

void writeBlock(const char *src, const BlockID &id, unsigned int start,
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