#include <Interpreter/Token.h>
#include <stdexcept>

namespace Interpreter {

Token::Token(const Keyword &key, const int nl, const int nc)
    : nl(nl), nc(nc), type(TokenType::keyword) {
    value.keyval = key;
}

Token::Token(const std::string &str, const TokenType type, const int nl,
             const int nc)
    : nl(nl), nc(nc), type(type) {
    if (type == TokenType::identifier) {
        value.strval = str;
    } else if (type == TokenType::string) {
        value.strval = str;
    } else {
        throw std::logic_error("illegal TokenType");
    }
}

Token::Token(const Symbol &sym, const int nl, const int nc)
    : nl(nl), nc(nc), type(TokenType::symbol) {
    value.symval = sym;
}

Token::Token(const int num, const int nl, const int nc)
    : nl(nl), nc(nc), type(TokenType::integer) {
    value.intval = num;
}

Token::Token(const float num, const int nl, const int nc)
    : nl(nl), nc(nc), type(TokenType::floating) {
    value.floatval = num;
}

TokenType Token::getType() const { return type; }

TokenValue Token::getValue() const { return value; }

int Token::getNl() const { return nl; }

int Token::getNc() const { return nc; }

} // namespace Interpreter