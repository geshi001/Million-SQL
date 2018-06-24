#pragma once
#include <BufferManager/BufferManager.h>
#include <DataType.h>
#include <FileSpec.h>

namespace CatalogManager {

constexpr size_t NAME_LENGTH = 64;

namespace BM = BufferManager;

void init();
void exit();

void createTable(const std::string &, const std::string &,
                 const std::vector<Attribute> &);

} // namespace CatalogManager
