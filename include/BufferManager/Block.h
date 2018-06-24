#pragma once
#include <cstdint>
#include <memory>
#include <string>

namespace BufferManager {

constexpr int BLOCK_SIZE = 4096; // 4KB

using BlockID = std::pair<std::string, uint32_t>;

class BufferManager;

class Block {
    friend class BufferManager;

  private:
    char block_data[BLOCK_SIZE];
    std::string filename;
    uint32_t offset;
    bool free, dirty, pinned;
    Block(const Block &) = delete;
    Block &operator=(const Block &) = delete;

  public:
    Block(const BlockID &);
    void read();
    void write();
};

using PtrBlock = std::shared_ptr<Block>;

} // namespace BufferManager