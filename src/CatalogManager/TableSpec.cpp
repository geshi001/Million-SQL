#include <CatalogManager/CatalogManager.h>

#define INT_t 0x0100u
#define FLOAT_t 0x0200u
#define CHAR_t 0x0400u
#define CHAR_MASK 0xFFu
#define UNIQUE 0x1000u

namespace CatalogManager {

uint32_t binarySize(const Schema &schema) {
    uint32_t cnt = sizeof(uint32_t) + sizeof(uint32_t);
    cnt += NAME_LENGTH;
    for (auto &attribute : schema.attributes) {
        cnt += NAME_LENGTH;
        cnt += sizeof(uint32_t);
    }
}

uint32_t encodeProperties(const Attribute &attribute) {
    uint32_t bin = 0u;
    switch (attribute.type) {
    case ValueType::INT: {
        bin |= INT_t;
        break;
    }
    case ValueType::FLOAT: {
        bin |= FLOAT_t;
        break;
    }
    case ValueType::CHAR: {
        bin |= CHAR_t;
        bin |= 0xFFu & attribute.charCnt;
        break;
    }
    }
    if (attribute.isUnique) {
        bin |= UNIQUE;
    }
    return bin;
}

std::tuple<ValueType, size_t, bool> decodeProperties(const uint32_t bin) {
    ValueType type;
    size_t charCnt = 0;
    if (bin & INT_t) {
        type = ValueType::INT;
    } else if (bin & FLOAT_t) {
        type = ValueType::FLOAT;
    } else if (bin & CHAR_t) {
        type = ValueType::CHAR;
        charCnt = bin & CHAR_MASK;
    }
    bool isUnique = (bin & UNIQUE) ? true : false;
    return std::make_tuple(type, charCnt, isUnique);
}

} // namespace CatalogManager