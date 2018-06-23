#pragma once
#include <string>

namespace Interpreter {

enum class Keyword {
    AND,
    CHAR,
    CREATE,
    DELETE,
    DROP,
    EXECFILE,
    FLOAT,
    FROM,
    INDEX,
    INSERT,
    INT,
    INTO,
    KEY,
    ON,
    PRIMARY,
    QUIT,
    SELECT,
    TABLE,
    UNIQUE,
    VALUES,
    WHERE
};

enum class Symbol {
    LPAREN,
    RPAREN,
    SEMI,
    COMMA,
    EQ,
    LT,
    LEQ,
    NE,
    GT,
    GEQ,
    ASTERISK
};

const char *mapKeywordToString(const Keyword &);
const char *mapSymbolToString(const Symbol &);

enum class TokenType { keyword, identifier, symbol, integer, floating, string };

struct TokenValue {
    Keyword keyval;
    std::string strval;
    Symbol symval;
    int intval;
    float floatval;
};

class Token {
  public:
    Token(const Keyword &, const int nl, const int nc);
    Token(const std::string &, const TokenType, const int nl, const int nc);
    Token(const Symbol &, const int nl, const int nc);
    Token(const int, const int nl, const int nc);
    Token(const float, const int nl, const int nc);
    TokenType getType() const;
    TokenValue getValue() const;
    int getNl() const;
    int getNc() const;

  private:
    TokenType type;
    TokenValue value;
    int nl, nc;
};

} // namespace Interpreter