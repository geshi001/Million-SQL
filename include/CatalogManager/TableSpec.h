#pragma once
#include <DataType.h>
#include <cstdint>
#include <tuple>

namespace CatalogManager {

uint32_t binarySize(const Schema &);
uint32_t encodeProperties(const Attribute &);
std::tuple<ValueType, size_t, bool> decodeProperties(const uint32_t);

} // namespace CatalogManager