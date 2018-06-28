#pragma once
#include <BufferManager/Block.h>
#include <FileSpec.h>
#include <cstdint>

namespace BM {

const size_t CACHE_SIZE = 1024;

void init();
void exit();

bool fileExists(const std::string &);

void createFile(const std::string &, const File::FileType);

void deleteFile(const std::string &);

PtrBlock readBlock(const BlockID &);

void writeBlock(const BlockID &, const char *src, uint32_t start, size_t size);

} // namespace BM