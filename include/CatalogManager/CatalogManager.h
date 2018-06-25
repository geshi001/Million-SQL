#pragma once
#include <BufferManager/BufferManager.h>
#include <DataType.h>
#include <FileSpec.h>
#include <list>

namespace CatalogManager {

constexpr size_t NAME_LENGTH = 64;

namespace BM = BufferManager;

extern std::list<std::shared_ptr<Schema>> schemas;

void init();
void exit();

void createTable(const std::string &, const std::string &,
                 const std::vector<Attribute> &);

} // namespace CatalogManager
