#pragma once
#include <DataType.h>
#include <cstdint>
#include <tuple>

namespace CatalogManager {

uint32_t binarySize(const Table &);
uint32_t encodeProperties(const Attribute &attribute);
std::tuple<ValueType, size_t, bool> decodeProperties(const uint32_t bin);

} // namespace CatalogManager