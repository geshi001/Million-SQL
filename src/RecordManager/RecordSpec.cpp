#include <RecordManager/RecordSpec.h>

namespace RM {

uint32_t recordBinarySize(const Schema &schema) {
    uint32_t cnt = sizeof(uint32_t); // pointer to next record
    for (auto &attribute : schema.attributes) {
        cnt += attribute.size();
    }
    return cnt;
}

uint32_t recordBinarySize(const Record &record) {
    uint32_t cnt = sizeof(uint32_t); // pointer to next record
    for (auto &value : record) {
        cnt += value.size();
    }
    return cnt;
}

} // namespace RM