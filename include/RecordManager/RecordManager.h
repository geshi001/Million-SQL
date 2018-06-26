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
void deleteAllRecords(const std::string &);
void deleteRecords(const std::string &, const std::vector<uint32_t> &);
std::vector<Record> selectRecords(std::shared_ptr<Schema>,
                                  const std::vector<Predicate> &);
std::vector<Record> project(const std::vector<Record> &,
                            std::shared_ptr<Schema>,
                            const std::vector<std::string> &);

} // namespace RecordManager
