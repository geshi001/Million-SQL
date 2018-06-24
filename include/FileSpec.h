#pragma once
#include <string>

namespace File {

const char *catalogFilename = "minisql.ctl";

inline std::string tableFilename(const std::string &name) {
    return "minisql_" + name + ".tbl";
}

inline std::string indexFilename(const std::string &name) {
    return "minisql_" + name + ".idx";
}

}; // namespace File
