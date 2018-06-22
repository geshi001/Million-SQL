#include <Interpreter/Lexer.h>
#include <Interpreter/ParseError.h>

namespace Interpreter {

Token::Token(const std::string &text, const int nl, const int nc)
    : text(text), nl(nl), nc(nc) {}

std::string Token::getText() const { return text; }

int Token::getNl() const { return nl; }

int Token::getNc() const { return nc; }

} // namespace Interpreter