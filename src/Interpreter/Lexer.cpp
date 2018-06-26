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
    if (p == currLine.end() || !skipSpaces()) {
        throw end_of_file::eof;
    }
    int onl = nl, onc = nc;
    if (*p == '(') {
        skipChars(1);
        return Token(Symbol::LPAREN, onl, onc);
    } else if (*p == ')') {
        skipChars(1);
        return Token(Symbol::RPAREN, onl, onc);
    } else if (*p == ';') {
        skipChars(1);
        return Token(Symbol::SEMI, onl, onc);
    } else if (*p == ',') {
        skipChars(1);
        return Token(Symbol::COMMA, onl, onc);
    } else if (*p == '=') {
        skipChars(1);
        return Token(Symbol::EQ, onl, onc);
    } else if (*p == '*') {
        skipChars(1);
        return Token(Symbol::ASTERISK, onl, onc);
    } else if (*p == '<') {
        skipChars(1);
        if (p != currLine.end()) {
            if (*p == '=') {
                skipChars(1);
                return Token(Symbol::LEQ, onl, onc);
            } else if (*p == '>') {
                skipChars(1);
                return Token(Symbol::NE, onl, onc);
            }
        }
        return Token(Symbol::LT, onl, onc);
    } else if (*p == '>') {
        skipChars(1);
        if (p != currLine.end()) {
            if (*p == '=') {
                skipChars(1);
                return Token(Symbol::GEQ, onl, onc);
            }
        }
        return Token(Symbol::GT, onl, onc);
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
        if (n >= 64) {
            throw ParseError(
                "identifiers with more than 63 characters are not supported",
                onl, onc);
        }
        if (str == "and") {
            return Token(Keyword::AND, onl, onc);
        } else if (str == "char") {
            return Token(Keyword::CHAR, onl, onc);
        } else if (str == "create") {
            return Token(Keyword::CREATE, onl, onc);
        } else if (str == "delete") {
            return Token(Keyword::DELETE, onl, onc);
        } else if (str == "drop") {
            return Token(Keyword::DROP, onl, onc);
        } else if (str == "execfile") {
            return Token(Keyword::EXECFILE, onl, onc);
        } else if (str == "float") {
            return Token(Keyword::FLOAT, onl, onc);
        } else if (str == "from") {
            return Token(Keyword::FROM, onl, onc);
        } else if (str == "index") {
            return Token(Keyword::INDEX, onl, onc);
        } else if (str == "insert") {
            return Token(Keyword::INSERT, onl, onc);
        } else if (str == "int") {
            return Token(Keyword::INT, onl, onc);
        } else if (str == "into") {
            return Token(Keyword::INTO, onl, onc);
        } else if (str == "key") {
            return Token(Keyword::KEY, onl, onc);
        } else if (str == "on") {
            return Token(Keyword::ON, onl, onc);
        } else if (str == "primary") {
            return Token(Keyword::PRIMARY, onl, onc);
        } else if (str == "quit") {
            return Token(Keyword::QUIT, onl, onc);
        } else if (str == "select") {
            return Token(Keyword::SELECT, onl, onc);
        } else if (str == "table") {
            return Token(Keyword::TABLE, onl, onc);
        } else if (str == "unique") {
            return Token(Keyword::UNIQUE, onl, onc);
        } else if (str == "values") {
            return Token(Keyword::VALUES, onl, onc);
        } else if (str == "where") {
            return Token(Keyword::WHERE, onl, onc);
        } else {
            return Token(str, TokenType::identifier, onl, onc);
        }
    } else if (std::isdigit(*p) || *p == '-') {
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
            return Token(floatval, onl, onc);
        }
        int intval = std::stoi(std::string(p, e));
        skipChars(n);
        return Token(intval, onl, onc);
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
            throw ParseError("multi-line strings not supported", nl, nc + n);
        }
        e++;
        n++;
        std::string str(p + 1, e - 1);
        skipChars(n);
        return Token(str, TokenType::string, onl, onc);
    } else {
        throw ParseError("unknown token", nl, nc);
    }
}

} // namespace Interpreter