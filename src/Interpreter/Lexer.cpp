#include <Interpreter/Lexer.h>
#include <Interpreter/ParseError.h>

namespace Interpreter {

Lexer::Lexer(std::istream &is) : is(is), nl(0), nc(0) { nextLine(); }

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    try {
        while (true) {
            tokens.push_back(nextToken());
        }
    } catch (end_of_file) {
        return tokens;
    }
}

bool Lexer::nextLine() {
    if (!std::getline(is, currLine)) {
        return false;
    }
    nl++;
    nc = 1;
    p = currLine.begin();
    return true;
}

void Lexer::skipChars(int n) {
    while (n--) {
        ++nc;
        ++p;
    }
}

bool Lexer::skipSpaces() {
    while (true) {
        if (p == currLine.end()) {
            if (!nextLine()) {
                return false;
            }
        } else if (std::isspace(*p)) {
            skipChars(1);
        } else {
            break;
        }
    }
    return true;
}

Token Lexer::nextToken() {
    if (!skipSpaces() || p == currLine.end()) {
        throw end_of_file::eof;
    }
    if (*p == '(') {
        skipChars(1);
        return Token(Symbol::LPAREN, nl, nc);
    } else if (*p == ')') {
        skipChars(1);
        return Token(Symbol::RPAREN, nl, nc);
    } else if (*p == ';') {
        skipChars(1);
        return Token(Symbol::SEMI, nl, nc);
    } else if (*p == ',') {
        skipChars(1);
        return Token(Symbol::COMMA, nl, nc);
    } else if (*p == '=') {
        skipChars(1);
        return Token(Symbol::EQ, nl, nc);
    } else if (*p == '*') {
        skipChars(1);
        return Token(Symbol::ASTERISK, nl, nc);
    } else if (*p == '<') {
        skipChars(1);
        if (p != currLine.end()) {
            if (*p == '=') {
                skipChars(1);
                return Token(Symbol::LEQ, nl, nc);
            } else if (*p == '>') {
                skipChars(1);
                return Token(Symbol::NE, nl, nc);
            }
        } else {
            return Token(Symbol::LT, nl, nc);
        }
    } else if (*p == '>') {
        skipChars(1);
        if (p != currLine.end()) {
            if (*p == '=') {
                skipChars(1);
                return Token(Symbol::GEQ, nl, nc);
            }
        } else {
            return Token(Symbol::GT, nl, nc);
        }
    } else if (std::isalpha(*p) || *p == '_') {
        int n = 1;
        auto e = p + 1;
        while (e != currLine.end()) {
            if (std::isalnum(*e) || *e == '_') {
                n++;
                e++;
            } else {
                break;
            }
        }
        std::string str(p, e);
        skipChars(n);
        if (str == "and") {
            return Token(Keyword::AND, nl, nc);
        } else if (str == "char") {
            return Token(Keyword::CHAR, nl, nc);
        } else if (str == "create") {
            return Token(Keyword::CREATE, nl, nc);
        } else if (str == "delete") {
            return Token(Keyword::DELETE, nl, nc);
        } else if (str == "drop") {
            return Token(Keyword::DROP, nl, nc);
        } else if (str == "execfile") {
            return Token(Keyword::EXECFILE, nl, nc);
        } else if (str == "float") {
            return Token(Keyword::FLOAT, nl, nc);
        } else if (str == "from") {
            return Token(Keyword::FROM, nl, nc);
        } else if (str == "index") {
            return Token(Keyword::INDEX, nl, nc);
        } else if (str == "insert") {
            return Token(Keyword::INSERT, nl, nc);
        } else if (str == "int") {
            return Token(Keyword::INT, nl, nc);
        } else if (str == "into") {
            return Token(Keyword::INTO, nl, nc);
        } else if (str == "key") {
            return Token(Keyword::KEY, nl, nc);
        } else if (str == "on") {
            return Token(Keyword::ON, nl, nc);
        } else if (str == "primary") {
            return Token(Keyword::PRIMARY, nl, nc);
        } else if (str == "quit") {
            return Token(Keyword::QUIT, nl, nc);
        } else if (str == "select") {
            return Token(Keyword::SELECT, nl, nc);
        } else if (str == "table") {
            return Token(Keyword::TABLE, nl, nc);
        } else if (str == "unique") {
            return Token(Keyword::UNIQUE, nl, nc);
        } else if (str == "values") {
            return Token(Keyword::VALUES, nl, nc);
        } else if (str == "where") {
            return Token(Keyword::WHERE, nl, nc);
        } else {
            return Token(str, TokenType::identifier, nl, nc);
        }
    } else if (std::isdigit(*p)) {
        int n = 1;
        auto e = p + 1;
        while (e != currLine.end()) {
            if (std::isdigit(*e)) {
                n++;
                e++;
            } else {
                break;
            }
        }
        if (e != currLine.end() && *e == '.') {
            n++;
            e++;
            while (e != currLine.end()) {
                if (std::isdigit(*e)) {
                    n++;
                    e++;
                } else {
                    break;
                }
            }
            float floatval = std::stof(std::string(p, e));
            skipChars(n);
            return Token(floatval, nl, nc);
        }
        int intval = std::stoi(std::string(p, e));
        skipChars(n);
        return Token(intval, nl, nc);
    } else if (*p == '\'') {
        int n = 1;
        auto e = p + 1;
        while (e != currLine.end()) {
            if (*e != '\'') {
                n++;
                e++;
            } else {
                break;
            }
        }
        if (*e != '\'') {
            throw ParseError("multi-line strings not supported", nl, nc);
        }
        e++;
        n++;
        std::string str(p + 1, e - 1);
        skipChars(n);
        return Token(str, TokenType::string, nl, nc);
    } else {
        throw ParseError("unknown token", nl, nc);
    }
}

} // namespace Interpreter