#pragma once
#include <DataType.h>

namespace RecordManager {

uint32_t recordBinarySize(const Schema &);
uint32_t recordBinarySize(const Record &);

} // namespace RecordManager