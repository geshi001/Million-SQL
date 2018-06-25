#include <BufferManager/Block.h>
#include <FileSpec.h>
#include <cstdint>
#include <cstring>
#include <fstream>
using namespace std;

namespace BufferManager {

Block::Block(const BlockID &id)
    : filename(id.first), offset(id.second), free(true), dirty(false),
      pinned(false), pos(0) {}

void Block::read(char *dest, size_t size) {
    std::memcpy(dest, block_data + pos, size);
    pos += size;
};

void Block::readFile() {
    std::ifstream ifs;
    ifs.open(filename, std::ios::in | std::ios::binary);
    ifs.seekg(BLOCK_SIZE * offset, std::ios::beg);
    ifs.read(block_data, BLOCK_SIZE);
    ifs.close();
}

void Block::writeFile() {
    std::fstream fs;
    fs.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    fs.seekp(BLOCK_SIZE * offset, std::ios::beg);
    fs.write(block_data, BLOCK_SIZE);
    fs.close();
}

void Block::createFile() {
    std::ofstream ofs;
    ofs.open(filename, std::ios::out | std::ios::binary);
    ofs.seekp(BLOCK_SIZE * offset, std::ios::beg);
    ofs.write(block_data, BLOCK_SIZE);
    ofs.flush();
    ofs.close();
}

} // namespace BufferManager