#pragma once
#include <BufferManager/BufferManager.h>

namespace IM {

void init();
void exit();

void createIndex(const std::string &, const std::string &, const std::string &);

void dropIndex(const std::string &);

} // namespace IM
