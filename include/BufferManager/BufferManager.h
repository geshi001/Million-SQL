#pragma once
#include <BufferManager/Block.h>
#include <cstring>
#include <list>
#include <memory>
#include <set>

namespace BufferManager {

const int CACHE_SIZE = 1024;

class BufferManager {
  private:
    BufferManager() = default;
    std::list<PtrBlock> cache;

  public:
    static BufferManager &manager() {
        static BufferManager instance;
        return instance;
    }

    PtrBlock readBlock(const BlockID &id);

    void writeBlock(const char *src, const BlockID &id, unsigned int start,
                    size_t size);
};

} // namespace BufferManager