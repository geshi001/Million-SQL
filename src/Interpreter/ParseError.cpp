#include <Interpreter/ParseError.h>
#include <sstream>

namespace Interpreter {

static std::string errMsg(const std::string &what_arg, const int nl,
                          const int nc) {
    std::stringstream ss;
    if (nl != -1 && nc != -1) {
        ss << "Line " << nl << ", Column " << nc << ":" << std::endl;
    } else {
        ss << "At the end of input:" << std::endl;
    }
    ss << "\033[31mParseError: \033[0m" << what_arg;
    return ss.str();
}

ParseError::ParseError(const std::string &what_arg, const int nl, const int nc)
    : std::runtime_error(errMsg(what_arg, nl, nc)) {}

} // namespace Interpreter