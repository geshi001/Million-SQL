#include <BufferManager/Block.h>
#include <FileSpec.h>
#include <cstdint>
#include <cstring>
#include <fstream>
using namespace std;

namespace BufferManager {

Block::Block(const BlockID &id)
    : filename(id.first), offset(id.second), free(true), dirty(false),
      pinned(false) {}

void Block::read() {
    std::ifstream ifs;
    ifs.open(filename, std::ios::in | std::ios::binary);
    ifs.seekg(BLOCK_SIZE * offset, std::ios::beg);
    ifs.read(block_data, BLOCK_SIZE);
    ifs.close();
}

void Block::write() {
    std::ofstream ofs;
    ofs.open(filename, std::ios::out | std::ios::binary);
    ofs.seekp(BLOCK_SIZE * offset, std::ios::beg);
    ofs.write(block_data, BLOCK_SIZE);
    ofs.flush();
    ofs.close();
}

} // namespace BufferManager