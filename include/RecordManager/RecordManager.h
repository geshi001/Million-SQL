#pragma once
#include <BufferManager/BufferManager.h>
#include <CatalogManager/CatalogManager.h>
#include <DataType.h>
#include <string>

namespace RecordManager {

namespace BM = BufferManager;
namespace CM = CatalogManager;

void init();
void exit();

void loadTable(const std::string &tableName);

} // namespace RecordManager
