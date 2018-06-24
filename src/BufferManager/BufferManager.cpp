#include <BufferManager/BufferManager.h>

namespace BufferManager {

PtrBlock BufferManager::readBlock(const BlockID &id) {
    for (auto iter = cache.begin(); iter != cache.end(); ++iter) {
        auto blkPtr = *iter;
        if (blkPtr->filename == id.first && blkPtr->offset == id.second) {
            cache.erase(iter);
            cache.push_front(blkPtr);
            return blkPtr;
        }
    }
    while (cache.size() >= CACHE_SIZE) {
        for (auto iter = cache.rbegin(); iter != cache.rend(); ++iter) {
            auto blkPtr = *iter;
            if (blkPtr->pinned) {
                continue;
            } else if (blkPtr->dirty) {
                blkPtr->write();
                blkPtr->dirty = false;
            }
            cache.erase(std::next(iter).base()); // erase(iter)
            break;
        }
    }
    auto blkPtr = std::make_shared<Block>(id);
    blkPtr->read();
    blkPtr->free = false;
    cache.push_front(blkPtr);
    return blkPtr;
}

void BufferManager::writeBlock(const char *src, const BlockID &id,
                               unsigned int start, size_t size) {
    for (auto blkPtr : cache) {
        if (blkPtr->filename == id.first && blkPtr->offset == id.second) {
            blkPtr->dirty = true;
            std::memcpy(blkPtr->block_data + start, src, size);
            return;
        }
    }
    auto blkPtr = readBlock(id);
    blkPtr->dirty = true;
    std::memcpy(blkPtr->block_data + start, src, size);
    return;
}

} // namespace BufferManager