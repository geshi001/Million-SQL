#pragma once
#include <cstdint>
#include <string>

namespace File {

enum class FileType { CATALOG = 0x7ACA, TABLE = 0xB17A, INDEX = 0xE81D };

inline std::string catalogFilename() { return "dbms/minisql.ctl"; }

inline std::string tableFilename(const std::string &name) {
    return "dbms/minisql_" + name + ".tbl";
}

inline std::string indexFilename(const std::string &name) {
    return "dbms/minisql_" + name + ".idx";
}

struct catalogFileHeader {
    uint32_t filetype;
    uint32_t blockNum;
    uint32_t tableOffset;
    uint32_t indexOffset;
};

struct tableFileHeader {
    uint32_t filetype;
    uint32_t blockNum;
    uint32_t beginOffset;
    uint32_t deletedOffset;
    uint32_t availableOffset;
};

}; // namespace File
