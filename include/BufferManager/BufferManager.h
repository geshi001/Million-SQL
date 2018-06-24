#pragma once
#include <BufferManager/Block.h>
#include <FileSpec.h>

namespace BufferManager {

const int CACHE_SIZE = 1024;

void init();

bool fileExists(const std::string &);

void createFile(const std::string &, const File::FileType);

PtrBlock readBlock(const BlockID &);

void writeBlock(const char *src, const BlockID &id, unsigned int start,
                size_t size);

} // namespace BufferManager