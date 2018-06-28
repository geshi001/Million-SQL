#pragma once
#include <BufferManager/BufferManager.h>
#include <DataType.h>
#include <FileSpec.h>
#include <unordered_map>

namespace CM {

constexpr size_t NAME_LENGTH = 64;

extern std::unordered_map<std::string, std::shared_ptr<Schema>> mapSchemas;
extern std::unordered_map<std::string, Index> mapIndices;

void init();
void exit();

bool hasTable(const std::string &);

void createTable(const std::string &, const std::string &,
                 const std::vector<Attribute> &);

void dropTable(const std::string &);

void createIndex(const std::string &, const std::string &, const std::string &);

void dropIndex(const std::string &);

void checkPredicates(const std::string &, const std::vector<Predicate> &);

std::shared_ptr<Schema> getSchema(const std::string &);

} // namespace CM
