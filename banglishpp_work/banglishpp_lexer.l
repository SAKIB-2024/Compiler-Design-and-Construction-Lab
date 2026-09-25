%option noyywrap nodefault yylineno

%{
#include "part01_token.hpp"
#include <string>
#include <vector>
using namespace std;

static vector<Token>* gTokens = nullptr;
static vector<string>* gErrors = nullptr;
static int gColumn = 1;

static void emitToken(TokenType type, const string& lexeme) {
    gTokens->emplace_back(type, lexeme, yylineno, gColumn);
    gColumn += yyleng;
}

static void lexError(const string& msg) {
    gErrors->push_back("Lexer error at line " + to_string(yylineno) +
                       ", column " + to_string(gColumn) + ": " + msg);
}

static string decodeString(const char* text, int len) {
    string value;
    for (int i = 1; i < len - 1; ++i) {
        char c = text[i];
        if (c == '\\' && i + 1 < len - 1) {
            char e = text[++i];
            switch (e) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case '"': value += '"'; break;
                case '\\': value += '\\'; break;
                default: value += e; break;
            }
        } else {
            value += c;
        }
    }
    return value;
}

static TokenType keywordType(const string& text) {
    static const unordered_map<string, TokenType> keywords = {
        {"shonkha", TokenType::KW_SHONKHA},
        {"doshomik", TokenType::KW_DOSHOMIK},
        {"shobdo", TokenType::KW_SHOBDO},
        {"jodi", TokenType::KW_JODI},
        {"nahole", TokenType::KW_NAHOLE},
        {"jokhon", TokenType::KW_JOKHON},
        {"lehkho", TokenType::KW_LEHKHO},
        {"infobro", TokenType::KW_INFOBRO},
        {"cholo", TokenType::KW_CHOLO},
        {"thamvai", TokenType::KW_THAMVAI}
    };
    auto it = keywords.find(text);
    return it == keywords.end() ? TokenType::IDENTIFIER : it->second;
}
%}

DIGIT       [0-9]
ID          [A-Za-z_][A-Za-z0-9_]*
INT         {DIGIT}+
FLOAT       {DIGIT}+\.{DIGIT}+

%%

[ \t\r]+                         { gColumn += yyleng; }
\n                                { gColumn = 1; }
\#[^\n]*                        { gColumn += yyleng; }
\/\/[^\n]*                      { gColumn += yyleng; }

{FLOAT}                          { emitToken(TokenType::FLOAT_LITERAL, yytext); }
{INT}                            { emitToken(TokenType::INTEGER_LITERAL, yytext); }
\"([^\\\"\n]|\\.)*\"          { emitToken(TokenType::STRING_LITERAL, decodeString(yytext, yyleng)); }
{ID}                             { emitToken(keywordType(yytext), yytext); }

"=="                            { emitToken(TokenType::EQUAL_EQUAL, yytext); }
"!="                            { emitToken(TokenType::NOT_EQUAL, yytext); }
"<="                            { emitToken(TokenType::LESS_EQUAL, yytext); }
">="                            { emitToken(TokenType::GREATER_EQUAL, yytext); }
"&&"                            { emitToken(TokenType::AND_AND, yytext); }
"||"                            { emitToken(TokenType::OR_OR, yytext); }
"="                             { emitToken(TokenType::ASSIGN, yytext); }
"<"                             { emitToken(TokenType::LESS, yytext); }
">"                             { emitToken(TokenType::GREATER, yytext); }
"+"                             { emitToken(TokenType::PLUS, yytext); }
"-"                             { emitToken(TokenType::MINUS, yytext); }
"*"                             { emitToken(TokenType::STAR, yytext); }
"/"                             { emitToken(TokenType::SLASH, yytext); }
"%"                             { emitToken(TokenType::PERCENT, yytext); }
"("                             { emitToken(TokenType::LPAREN, yytext); }
")"                             { emitToken(TokenType::RPAREN, yytext); }
"{"                             { emitToken(TokenType::LBRACE, yytext); }
"}"                             { emitToken(TokenType::RBRACE, yytext); }
";"                             { emitToken(TokenType::SEMICOLON, yytext); }
"!"                             { lexError("Unexpected '!'; expected '!='"); ++gColumn; }
"&"                             { lexError("Unexpected '&'; expected '&&'"); ++gColumn; }
"|"                             { lexError("Unexpected '|'; expected '||'"); ++gColumn; }
\"([^\\\"\n]|\\.)*              { lexError("Unterminated string literal"); gColumn += yyleng; }
.                                { lexError(string("Unexpected character '") + yytext[0] + "'"); ++gColumn; }

%%

vector<Token> lexWithFlex(const string& source, vector<string>& errors) {
    vector<Token> tokens;
    gTokens = &tokens;
    gErrors = &errors;
    gColumn = 1;
    yylineno = 1;

    YY_BUFFER_STATE buffer = yy_scan_bytes(source.data(), static_cast<int>(source.size()));
    yylex();
    yy_delete_buffer(buffer);

    tokens.emplace_back(TokenType::END_OF_FILE, "", yylineno, gColumn);
    gTokens = nullptr;
    gErrors = nullptr;
    return tokens;
}
