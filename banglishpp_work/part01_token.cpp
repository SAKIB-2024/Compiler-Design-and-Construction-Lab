#include <bits/stdc++.h>
using namespace std;

#define ll long long

// ================================================================
// BangLish++ Compiler
// Pipeline: Source -> Lexer -> Tokens -> Parser/AST -> Type Checker
//          -> Python Code Generator
//
// Supported features:
//   shonkha  : integer
//   doshomik : double / floating-point
//   shobdo   : string
//   jodi / nahole / nahole jodi : if / else-if / else
//   jokhon   : while
//   lehkho(expr)  : print(expr)
//   infobro()     : input()
//   + - * / % with precedence
//   < > <= >= == != comparisons
//   && || logical operators
//   cholo / thamvai : continue / break inside jokhon loops
//   && / || logical operators
//   && logical AND, || logical OR
//   cholo : continue (only inside jokhon)
//   thamvai : break (only inside jokhon)
//   = assignment
//   // and # comments
// ================================================================

// ------------------------------- Token ---------------------------
enum class TokenType {
    END_OF_FILE,
    IDENTIFIER,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,

    KW_SHONKHA,
    KW_DOSHOMIK,
    KW_SHOBDO,
    KW_JODI,
    KW_NAHOLE,
    KW_JOKHON,
    KW_LEHKHO,
    KW_INFOBRO,
    KW_CHOLO,
    KW_THAMVAI,

    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,

    ASSIGN,
    EQUAL_EQUAL,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    AND_AND,
    OR_OR,

    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    SEMICOLON,
};

struct Token {
    TokenType type;
    string lexeme;
    int line;
    int column;

    Token(TokenType type, string lexeme, int line, int column)
        : type(type), lexeme(std::move(lexeme)), line(line), column(column) {}
};

static string tokenTypeName(TokenType t) {
    switch (t) {
        case TokenType::END_OF_FILE: return "end of file";
        case TokenType::IDENTIFIER: return "identifier";
        case TokenType::INTEGER_LITERAL: return "integer literal";
        case TokenType::FLOAT_LITERAL: return "float literal";
        case TokenType::STRING_LITERAL: return "string literal";
        case TokenType::KW_SHONKHA: return "shonkha";
        case TokenType::KW_DOSHOMIK: return "doshomik";
        case TokenType::KW_SHOBDO: return "shobdo";
        case TokenType::KW_JODI: return "jodi";
        case TokenType::KW_NAHOLE: return "nahole";
        case TokenType::KW_JOKHON: return "jokhon";
        case TokenType::KW_LEHKHO: return "lehkho";
        case TokenType::KW_INFOBRO: return "infobro";
        case TokenType::KW_CHOLO: return "cholo";
        case TokenType::KW_THAMVAI: return "thamvai";
        case TokenType::PLUS: return "+";
        case TokenType::MINUS: return "-";
        case TokenType::STAR: return "*";
        case TokenType::SLASH: return "/";
        case TokenType::PERCENT: return "%";
        case TokenType::ASSIGN: return "=";
        case TokenType::EQUAL_EQUAL: return "==";
        case TokenType::NOT_EQUAL: return "!=";
        case TokenType::LESS: return "<";
        case TokenType::LESS_EQUAL: return "<=";
        case TokenType::GREATER: return ">";
        case TokenType::GREATER_EQUAL: return ">=";
        case TokenType::AND_AND: return "&&";
        case TokenType::OR_OR: return "||";
        case TokenType::LPAREN: return "(";
        case TokenType::RPAREN: return ")";
        case TokenType::LBRACE: return "{";
        case TokenType::RBRACE: return "}";
        case TokenType::SEMICOLON: return ";";
    }
    return "unknown token";
}
