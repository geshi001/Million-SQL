#include <Interpreter/ParseError.h>
#include <sstream>

namespace Interpreter {

ParseError::ParseError(const std::string &what_arg, const int nl, const int nc)
    : std::runtime_error(what_arg), nl(nl), nc(nc) {}

const char *ParseError::what() const noexcept {
    std::stringstream ss;
    if (nl != -1 && nc != -1) {
        ss << "Line " << nl << ", Column " << nc << ":" << std::endl;
    }
    ss << "ParseError: " << std::runtime_error::what();
    return ss.str().c_str();
}

} // namespace Interpreter