#include <Interpreter/Parser.h>
#include <cassert>
#include <iostream>
#include <sstream>

namespace Interpreter {

Parser::Parser(std::istream &is) : lexer(is) {
    tokens = lexer.tokenize();
    p = tokens.begin();
}

Statements Parser::parse() {
    Statements stmts;
    std::cout << "start parsing, ";
    std::cout << std::distance(tokens.begin(), p) << "\n";
    while (p != tokens.end()) {
        auto &token = *p;
        if (token.getType() == TokenType::keyword) {
            auto keyword = token.getValue().keyval;
            if (keyword == Keyword::CREATE) {
                stmts.push_back(parseCreate());
            } else if (keyword == Keyword::DROP) {
                stmts.push_back(parseDrop());
            } else {
                return stmts;
            }
        }
        std::cout << "parsed one statement, ";
        std::cout << std::distance(tokens.begin(), p) << "\n";
    }
    return stmts;
}

PtrStmt Parser::parseCreate() {
    auto &token = *++p; // skip 'create'
    if (token.getType() == TokenType::keyword) {
        auto keyword = token.getValue().keyval;
        if (keyword == Keyword::TABLE) {
            return parseCreateTable();
        } else if (keyword == Keyword::INDEX) {
            return parseCreateIndex();
        }
    }
    raise("cannot parse create statement");
}

PtrStmt Parser::parseDrop() {
    auto &token = *++p; // skip 'drop'
    if (token.getType() == TokenType::keyword) {
        auto keyword = token.getValue().keyval;
        if (keyword == Keyword::TABLE) {
            return parseDropTable();
        } else if (keyword == Keyword::INDEX) {
            return parseDropIndex();
        }
    }
    raise("cannot parse create statement");
}

PtrStmt Parser::parseCreateTable() {
    ++p; // skip 'table'
    auto pStmt = std::make_shared<AST::CreateTableStatement>();
    pStmt->setTableName(getIdentifier());
    expect(Symbol::LPAREN);
    getTableDefns(pStmt);
    while (check(Symbol::COMMA)) {
        ++p;
        getTableDefns(pStmt);
    }
    expect(Symbol::RPAREN);
    expect(Symbol::SEMI);
    return pStmt;
}

PtrStmt Parser::parseDropTable() {
    ++p; // skip 'table'
    auto pStmt = std::make_shared<AST::DropTableStatement>();
    pStmt->setTableName(getIdentifier());
    expect(Symbol::SEMI);
    return pStmt;
}

PtrStmt Parser::parseCreateIndex() {
    ++p; // skip 'index'
    auto pStmt = std::make_shared<AST::CreateIndexStatement>();
    pStmt->setIndexName(getIdentifier());
    expect(Keyword::ON);
    pStmt->setTableName(getIdentifier());
    expect(Symbol::LPAREN);
    pStmt->setAttrName(getIdentifier());
    expect(Symbol::RPAREN);
    expect(Symbol::SEMI);
    return pStmt;
}

PtrStmt Parser::parseDropIndex() {
    ++p; // skip 'table'
    auto pStmt = std::make_shared<AST::DropIndexStatement>();
    pStmt->setTableName(getIdentifier());
    expect(Symbol::SEMI);
    return pStmt;
}

bool Parser::check(const Keyword &keyword) {
    return p != tokens.end() && p->getType() == TokenType::keyword &&
           p->getValue().keyval == keyword;
}

bool Parser::check(const Symbol &symbol) {
    return p != tokens.end() && p->getType() == TokenType::symbol &&
           p->getValue().symval == symbol;
}

void Parser::expect(const Keyword &keyword) {
    if (check(keyword)) {
        ++p;
    } else {
        std::stringstream ss;
        ss << "expecting \'";
        ss << mapKeywordToString(keyword);
        ss << "\'";
        raise(ss.str());
    }
}

void Parser::expect(const Symbol &symbol) {
    if (check(symbol)) {
        ++p;
    } else {
        std::stringstream ss;
        ss << "expecting \'";
        ss << mapSymbolToString(symbol);
        ss << "\'";
        raise(ss.str());
    }
}

std::string Parser::getIdentifier() {
    if (p != tokens.end() && p->getType() == TokenType::identifier) {
        return (p++)->getValue().strval;
    } else {
        raise("expecting identifier");
    }
}

std::pair<AttrType, size_t> Parser::getAttrType() {
    if (check(Keyword::INT)) {
        ++p;
        return std::make_pair(AttrType::INT, 0);
    } else if (check(Keyword::FLOAT)) {
        ++p;
        return std::make_pair(AttrType::FLOAT, 0);
    } else if (check(Keyword::CHAR)) {
        ++p;
        expect(Symbol::LPAREN);
        int size = getInteger();
        if (size < 1 || size > 255) {
            --p;
            throw ParseError("size of chars should be in [1, 255]", p->getNl(),
                             p->getNc());
        }
        expect(Symbol::RPAREN);
        return std::make_pair(AttrType::CHAR, static_cast<size_t>(size));
    } else {
        raise("expecting 'int', 'float', or 'char(..)'");
    }
}

void Parser::getTableDefns(std::shared_ptr<AST::CreateTableStatement> pStmt) {
    if (check(Keyword::PRIMARY)) {
        ++p;
        expect(Keyword::KEY);
        expect(Symbol::LPAREN);
        std::string primaryKey = getIdentifier();
        expect(Symbol::RPAREN);
        pStmt->addPrimaryKey(primaryKey);
    } else {
        Attribute attr;
        attr.attrName = getIdentifier();
        std::tie(attr.attrType, attr.charCnt) = getAttrType();
        if (check(Keyword::UNIQUE)) {
            attr.isUnique = true;
            ++p;
        } else {
            attr.isUnique = false;
        }
        pStmt->addAttribute(attr);
    }
};

int Parser::getInteger() {
    if (p != tokens.end() && p->getType() == TokenType::integer) {
        return (p++)->getValue().intval;
    } else {
        raise("expecting integer");
    }
}

float Parser::getFloating() {
    if (p != tokens.end() && p->getType() == TokenType::floating) {
        return (p++)->getValue().floatval;
    } else {
        raise("expecting floating");
    }
}

std::string Parser::getString() {
    if (p != tokens.end() && p->getType() == TokenType::string) {
        return (p++)->getValue().strval;
    } else {
        raise("expecting string");
    }
}

void Parser::raise(const std::string &what_arg) {
    if (p == tokens.end()) {
        throw ParseError(what_arg + "at end of input", -1, -1);
    } else {
        throw ParseError(what_arg, p->getNl(), p->getNc());
    }
}

} // namespace Interpreter