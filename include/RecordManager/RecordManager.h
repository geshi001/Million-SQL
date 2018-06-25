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

void createTable(const std::string &);
void dropTable(const std::string &);
void insertRecord(const std::string &, const Record &);
std::vector<Record> loadTable(std::shared_ptr<Schema>);

} // namespace RecordManager
