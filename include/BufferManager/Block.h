#pragma once
#include <cstdint>
#include <memory>
#include <string>

namespace BufferManager {

constexpr int BLOCK_SIZE = 4096; // 4KB

using BlockID = std::pair<std::string, uint32_t>;

inline BlockID makeID(const std::string &filename, const uint32_t offset) {
    return std::make_pair(filename, offset);
}

class Block {
  private:
    std::string filename;
    uint32_t offset;
    bool free, dirty, pinned;
    Block(const Block &) = delete;
    Block &operator=(const Block &) = delete;

  public:
    char block_data[BLOCK_SIZE];
    Block(const BlockID &);
    const std::string &getFilename() const { return filename; }
    const uint32_t getOffset() const { return offset; }
    inline bool isFree() const { return free; }
    inline bool isDirty() const { return dirty; }
    inline bool isPinned() const { return pinned; }
    inline void setFree(const bool value) { free = value; }
    inline void setDirty(const bool value) { dirty = value; }
    inline void setPinned(const bool value) { pinned = value; }
    void read();
    void write();
};

using PtrBlock = std::shared_ptr<Block>;

} // namespace BufferManager