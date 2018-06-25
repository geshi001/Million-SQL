#include <RecordManager/RecordSpec.h>

namespace RecordManager {

uint32_t recordBinarySize(const Schema &schema) {
    uint32_t cnt = sizeof(uint32_t); // pointer to next record
    for (auto &attribute : schema.attributes) {
        cnt += attribute.size();
    }
    return cnt;
}

} // namespace RecordManager