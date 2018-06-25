#include <Interpreter/Parser.h>
#include <cassert>
#include <cstring>
#include <sstream>

namespace Interpreter {

Parser::Parser(std::istream &is) : lexer(is) {
    tokens = lexer.tokenize();
    p = tokens.begin();
}

Statements Parser::parse() {
    Statements stmts;
    while (p != tokens.end()) {
        auto &token = *p;
        if (token.getType() == TokenType::keyword) {
            auto keyword = token.getValue().keyval;
            if (keyword == Keyword::CREATE) {
                stmts.push_back(parseCreate());
            } else if (keyword == Keyword::DROP) {
                stmts.push_back(parseDrop());
            } else if (keyword == Keyword::SELECT) {
                stmts.push_back(parseSelect());
            } else if (keyword == Keyword::INSERT) {
                stmts.push_back(parseInsert());
            } else if (keyword == Keyword::DELETE) {
                stmts.push_back(parseDelete());
            } else if (keyword == Keyword::QUIT) {
                stmts.push_back(parseQuit());
            } else if (keyword == Keyword::EXECFILE) {
                stmts.push_back(parseExecfile());
            } else {
                raise("unknown statement");
            }
        } else {
            raise("unknown statement");
        }
    }
    return stmts;
}

PtrStmt Parser::parseCreate() {
    skip(); // skip 'create'
    auto &token = *p;
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
    skip(); // skip 'drop'
    auto &token = *p;
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
    skip(); // skip 'table'
    auto pStmt = std::make_shared<AST::CreateTableStatement>();
    pStmt->setTableName(getIdentifier());
    expect(Symbol::LPAREN);
    getTableDefns(pStmt);
    while (check(Symbol::COMMA)) {
        skip(); // skip ','
        getTableDefns(pStmt);
    }
    expect(Symbol::RPAREN);
    expect(Symbol::SEMI);
    return pStmt;
}

PtrStmt Parser::parseDropTable() {
    skip(); // skip 'table'
    auto pStmt = std::make_shared<AST::DropTableStatement>();
    pStmt->setTableName(getIdentifier());
    expect(Symbol::SEMI);
    return pStmt;
}

PtrStmt Parser::parseCreateIndex() {
    skip(); // skip 'index'
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
    skip(); // skip 'table'
    auto pStmt = std::make_shared<AST::DropIndexStatement>();
    pStmt->setTableName(getIdentifier());
    expect(Symbol::SEMI);
    return pStmt;
}

PtrStmt Parser::parseSelect() {
    skip(); // skip 'select'
    auto pStmt = std::make_shared<AST::SelectStatement>();
    if (check(Symbol::ASTERISK)) {
        skip(); // skip '*'
    } else {
        pStmt->addAttrName(getIdentifier());
        while (check(Symbol::COMMA)) {
            skip(); // skip ','
            pStmt->addAttrName(getIdentifier());
        }
    }
    expect(Keyword::FROM);
    pStmt->setTableName(getIdentifier());
    if (check(Keyword::WHERE)) {
        skip(); // skip 'where'
        pStmt->addPredicate(getPredicate());
        while (check(Keyword::AND)) {
            skip(); // skip 'and'
            pStmt->addPredicate(getPredicate());
        }
    }
    expect(Symbol::SEMI);
    return pStmt;
}

PtrStmt Parser::parseInsert() {
    skip(); // skip 'insert'
    expect(Keyword::INTO);
    auto pStmt = std::make_shared<AST::InsertStatement>();
    pStmt->setTableName(getIdentifier());
    expect(Keyword::VALUES);
    expect(Symbol::LPAREN);
    pStmt->addValue(getValue());
    while (check(Symbol::COMMA)) {
        skip(); // skip ','
        pStmt->addValue(getValue());
    }
    expect(Symbol::RPAREN);
    expect(Symbol::SEMI);
    return pStmt;
}

PtrStmt Parser::parseDelete() {
    skip(); // skip 'delete'
    auto pStmt = std::make_shared<AST::DeleteStatement>();
    expect(Keyword::FROM);
    pStmt->setTableName(getIdentifier());
    if (check(Keyword::WHERE)) {
        skip(); // skip 'where'
        pStmt->addPredicate(getPredicate());
        while (check(Keyword::AND)) {
            skip(); // skip 'and'
            pStmt->addPredicate(getPredicate());
        }
    }
    expect(Symbol::SEMI);
    return pStmt;
}

PtrStmt Parser::parseQuit() {
    skip();
    auto pStmt = std::make_shared<AST::QuitStatement>();
    expect(Symbol::SEMI);
    return pStmt;
}

PtrStmt Parser::parseExecfile() {
    skip();
    auto pStmt = std::make_shared<AST::ExecfileStatement>();
    pStmt->setFilePath(getString());
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
        skip();
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
        skip();
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

std::pair<ValueType, size_t> Parser::getAttrType() {
    if (check(Keyword::INT)) {
        skip();
        return std::make_pair(ValueType::INT, 0);
    } else if (check(Keyword::FLOAT)) {
        skip();
        return std::make_pair(ValueType::FLOAT, 0);
    } else if (check(Keyword::CHAR)) {
        skip();
        expect(Symbol::LPAREN);
        int size = getInteger();
        if (size < 1 || size > 255) {
            --p;
            throw ParseError("size of chars should be in [1, 255]", p->getNl(),
                             p->getNc());
        }
        expect(Symbol::RPAREN);
        return std::make_pair(ValueType::CHAR, static_cast<size_t>(size));
    } else {
        raise("expecting 'int', 'float', or 'char(..)'");
    }
}

void Parser::getTableDefns(std::shared_ptr<AST::CreateTableStatement> pStmt) {
    if (check(Keyword::PRIMARY)) {
        skip();
        expect(Keyword::KEY);
        expect(Symbol::LPAREN);
        std::string primaryKey = getIdentifier();
        expect(Symbol::RPAREN);
        pStmt->addPrimaryKey(primaryKey);
    } else {
        Attribute attr;
        attr.name = getIdentifier();
        std::tie(attr.type, attr.charCnt) = getAttrType();
        if (check(Keyword::UNIQUE)) {
            attr.isUnique = true;
            skip();
        } else {
            attr.isUnique = false;
        }
        pStmt->addAttribute(attr);
    }
};

Predicate Parser::getPredicate() {
    Predicate predicate;
    predicate.attrName = getIdentifier();
    if (check(Symbol::EQ)) {
        predicate.op = OpType::EQ;
    } else if (check(Symbol::NE)) {
        predicate.op = OpType::NE;
    } else if (check(Symbol::LT)) {
        predicate.op = OpType::LT;
    } else if (check(Symbol::LEQ)) {
        predicate.op = OpType::LEQ;
    } else if (check(Symbol::GT)) {
        predicate.op = OpType::GT;
    } else if (check(Symbol::GEQ)) {
        predicate.op = OpType::GEQ;
    } else {
        raise(
            "expecting operators (one of '=', '<>', '<', '<=', '>', and '>=')");
    }
    skip();
    predicate.val = getValue();
    return predicate;
}

Value Parser::getValue() {
    if (p != tokens.end()) {
        Value value;
        if (p->getType() == TokenType::integer) {
            value.type = ValueType::INT;
            value.ival = p++->getValue().intval;
            return value;
        } else if (p->getType() == TokenType::floating) {
            value.type = ValueType::FLOAT;
            value.fval = p++->getValue().floatval;
            return value;
        } else if (p->getType() == TokenType::string) {
            auto str = p++->getValue().strval;
            if (str.length() >= 1 && str.length() <= 255) {
                value.type = ValueType::CHAR;
                value.charCnt = str.length();
                std::memset(value.cval, 0, 256);
                std::strcpy(value.cval, str.c_str());
                return value;
            } else {
                raise("length of string literal not appropriate");
            }
        }
    }
    raise("expecting value (int, float, or string)");
}

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

inline void Parser::skip() { ++p; }

} // namespace Interpreter