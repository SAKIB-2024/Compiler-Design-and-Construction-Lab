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
        : type(type), lexeme(move(lexeme)), line(line), column(column) {}
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

// ------------------------------- Lexer ---------------------------
class Lexer {
private:
    string source;
    size_t pos = 0;
    int line = 1;
    int column = 1;
    vector<string> &errors;

    unordered_map<string, TokenType> keywords {
        {"shonkha", TokenType::KW_SHONKHA},
        {"doshomik", TokenType::KW_DOSHOMIK},
        {"shobdo", TokenType::KW_SHOBDO},
        {"jodi", TokenType::KW_JODI},
        {"nahole", TokenType::KW_NAHOLE},
        {"jokhon", TokenType::KW_JOKHON},
        {"lehkho", TokenType::KW_LEHKHO},
        {"infobro", TokenType::KW_INFOBRO},
        {"cholo", TokenType::KW_CHOLO},
        {"thamvai", TokenType::KW_THAMVAI},
    };

    bool isAtEnd() const { return pos >= source.size(); }

    char peek() const {
        if (isAtEnd()) return '\0';
        return source[pos];
    }

    char peekNext() const {
        if (pos + 1 >= source.size()) return '\0';
        return source[pos + 1];
    }

    char advance() {
        if (isAtEnd()) return '\0';
        char c = source[pos++];
        if (c == '\n') {
            ++line;
            column = 1;
        } else {
            ++column;
        }
        return c;
    }

    void addError(const string &msg, int errLine, int errCol) {
        errors.push_back("Lexer error at line " + to_string(errLine) +
                         ", column " + to_string(errCol) + ": " + msg);
    }

    Token make(TokenType type, const string &text, int startLine, int startCol) {
        return Token(type, text, startLine, startCol);
    }

    Token identifierOrKeyword() {
        int startLine = line, startCol = column;
        size_t start = pos;
        while (isalnum(static_cast<unsigned char>(peek())) || peek() == '_') advance();
        string text = source.substr(start, pos - start);
        auto it = keywords.find(text);
        if (it != keywords.end()) return make(it->second, text, startLine, startCol);
        return make(TokenType::IDENTIFIER, text, startLine, startCol);
    }

    Token number() {
        int startLine = line, startCol = column;
        size_t start = pos;
        bool hasDot = false;

        while (isdigit(static_cast<unsigned char>(peek()))) advance();

        if (peek() == '.' && isdigit(static_cast<unsigned char>(peekNext()))) {
            hasDot = true;
            advance();
            while (isdigit(static_cast<unsigned char>(peek()))) advance();
        }

        string text = source.substr(start, pos - start);
        return make(hasDot ? TokenType::FLOAT_LITERAL : TokenType::INTEGER_LITERAL,
                    text, startLine, startCol);
    }

    Token stringLiteral() {
        int startLine = line, startCol = column;
        advance(); // opening quote
        string value;
        bool terminated = false;

        while (!isAtEnd()) {
            char c = peek();
            if (c == '"') {
                advance();
                terminated = true;
                break;
            }
            if (c == '\n') break;

            if (c == '\\') {
                advance();
                char e = peek();
                if (e == '\n' || e == '\0') break;
                switch (e) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case '"': value += '"'; break;
                    case '\\': value += '\\'; break;
                    default:
                        value += e;
                        break;
                }
                advance();
            } else {
                value += advance();
            }
        }

        if (!terminated) {
            addError("Unterminated string literal", startLine, startCol);
        }

        return make(TokenType::STRING_LITERAL, value, startLine, startCol);
    }

public:
    Lexer(string source, vector<string> &errors)
        : source(move(source)), errors(errors) {}

    vector<Token> tokenize() {
        vector<Token> tokens;

        while (!isAtEnd()) {
            char c = peek();

            if (isspace(static_cast<unsigned char>(c))) {
                advance();
                continue;
            }

            // Comments: //... or #...
            if (c == '#') {
                while (!isAtEnd() && peek() != '\n') advance();
                continue;
            }
            if (c == '/' && peekNext() == '/') {
                advance();
                advance();
                while (!isAtEnd() && peek() != '\n') advance();
                continue;
            }

            int startLine = line, startCol = column;

            if (isalpha(static_cast<unsigned char>(c)) || c == '_') {
                tokens.push_back(identifierOrKeyword());
                continue;
            }

            if (isdigit(static_cast<unsigned char>(c))) {
                tokens.push_back(number());
                continue;
            }

            if (c == '"') {
                tokens.push_back(stringLiteral());
                continue;
            }

            switch (c) {
                case '+': advance(); tokens.emplace_back(TokenType::PLUS, "+", startLine, startCol); break;
                case '-': advance(); tokens.emplace_back(TokenType::MINUS, "-", startLine, startCol); break;
                case '*': advance(); tokens.emplace_back(TokenType::STAR, "*", startLine, startCol); break;
                case '/': advance(); tokens.emplace_back(TokenType::SLASH, "/", startLine, startCol); break;
                case '%': advance(); tokens.emplace_back(TokenType::PERCENT, "%", startLine, startCol); break;
                case '(': advance(); tokens.emplace_back(TokenType::LPAREN, "(", startLine, startCol); break;
                case ')': advance(); tokens.emplace_back(TokenType::RPAREN, ")", startLine, startCol); break;
                case '{': advance(); tokens.emplace_back(TokenType::LBRACE, "{", startLine, startCol); break;
                case '}': advance(); tokens.emplace_back(TokenType::RBRACE, "}", startLine, startCol); break;
                case ';': advance(); tokens.emplace_back(TokenType::SEMICOLON, ";", startLine, startCol); break;

                case '=':
                    advance();
                    if (peek() == '=') {
                        advance();
                        tokens.emplace_back(TokenType::EQUAL_EQUAL, "==", startLine, startCol);
                    } else tokens.emplace_back(TokenType::ASSIGN, "=", startLine, startCol);
                    break;

                case '!':
                    advance();
                    if (peek() == '=') {
                        advance();
                        tokens.emplace_back(TokenType::NOT_EQUAL, "!=", startLine, startCol);
                    } else {
                        addError("Unexpected '!'; expected '!='", startLine, startCol);
                    }
                    break;

                case '<':
                    advance();
                    if (peek() == '=') {
                        advance();
                        tokens.emplace_back(TokenType::LESS_EQUAL, "<=", startLine, startCol);
                    } else tokens.emplace_back(TokenType::LESS, "<", startLine, startCol);
                    break;

                case '>':
                    advance();
                    if (peek() == '=') {
                        advance();
                        tokens.emplace_back(TokenType::GREATER_EQUAL, ">=", startLine, startCol);
                    } else tokens.emplace_back(TokenType::GREATER, ">", startLine, startCol);
                    break;

                case '&':
                    advance();
                    if (peek() == '&') {
                        advance();
                        tokens.emplace_back(TokenType::AND_AND, "&&", startLine, startCol);
                    } else {
                        addError("Unexpected '&'; expected '&&'", startLine, startCol);
                    }
                    break;

                case '|':
                    advance();
                    if (peek() == '|') {
                        advance();
                        tokens.emplace_back(TokenType::OR_OR, "||", startLine, startCol);
                    } else {
                        addError("Unexpected '|'; expected '||'", startLine, startCol);
                    }
                    break;

                default:
                    addError(string("Unexpected character '") + c + "'", startLine, startCol);
                    advance();
                    break;
            }
        }

        tokens.emplace_back(TokenType::END_OF_FILE, "", line, column);
        return tokens;
    }
};

// ---------------------------- AST Types --------------------------
enum class DataType { INT, DOUBLE, STRING, BOOL, ERROR };

static string dataTypeName(DataType t) {
    switch (t) {
        case DataType::INT: return "shonkha";
        case DataType::DOUBLE: return "doshomik";
        case DataType::STRING: return "shobdo";
        case DataType::BOOL: return "condition";
        case DataType::ERROR: return "error";
    }
    return "error";
}

struct Expr {
    virtual ~Expr() = default;
    DataType inferredType = DataType::ERROR;
};
using ExprPtr = unique_ptr<Expr>;

struct LiteralExpr : Expr {
    enum class Kind { INT, DOUBLE, STRING };
    Kind kind;
    string value;
    LiteralExpr(Kind kind, string value) : kind(kind), value(move(value)) {}
};

struct VariableExpr : Expr {
    string name;
    explicit VariableExpr(string name) : name(move(name)) {}
};

struct InputExpr : Expr {
    InputExpr() = default;
};

struct UnaryExpr : Expr {
    TokenType op;
    ExprPtr right;
    UnaryExpr(TokenType op, ExprPtr right) : op(op), right(move(right)) {}
};

struct BinaryExpr : Expr {
    ExprPtr left;
    TokenType op;
    ExprPtr right;
    BinaryExpr(ExprPtr left, TokenType op, ExprPtr right)
        : left(move(left)), op(op), right(move(right)) {}
};

struct Stmt {
    virtual ~Stmt() = default;
    int line = 0;
};
using StmtPtr = unique_ptr<Stmt>;

struct VarDeclStmt : Stmt {
    DataType type;
    string name;
    ExprPtr initializer;
    VarDeclStmt(DataType type, string name, ExprPtr initializer, int line)
        : type(type), name(move(name)), initializer(move(initializer)) { this->line = line; }
};

struct AssignmentStmt : Stmt {
    string name;
    ExprPtr value;
    AssignmentStmt(string name, ExprPtr value, int line)
        : name(move(name)), value(move(value)) { this->line = line; }
};

struct PrintStmt : Stmt {
    ExprPtr value;
    PrintStmt(ExprPtr value, int line) : value(move(value)) { this->line = line; }
};

struct ExprStmt : Stmt {
    ExprPtr value;
    ExprStmt(ExprPtr value, int line) : value(move(value)) { this->line = line; }
};

struct BlockStmt : Stmt {
    vector<StmtPtr> statements;
};

struct IfStmt : Stmt {
    ExprPtr condition;
    unique_ptr<BlockStmt> thenBranch;

    // `nahole jodi (...) { ... }` is represented as another IfStmt.
    // A final plain `nahole { ... }` is stored in elseBranch.
    unique_ptr<IfStmt> elseIf;
    unique_ptr<BlockStmt> elseBranch;

    IfStmt(ExprPtr condition,
           unique_ptr<BlockStmt> thenBranch,
           unique_ptr<IfStmt> elseIf,
           unique_ptr<BlockStmt> elseBranch,
           int line)
        : condition(move(condition)),
          thenBranch(move(thenBranch)),
          elseIf(move(elseIf)),
          elseBranch(move(elseBranch)) {
        this->line = line;
    }
};

struct WhileStmt : Stmt {
    ExprPtr condition;
    unique_ptr<BlockStmt> body;
    WhileStmt(ExprPtr condition, unique_ptr<BlockStmt> body, int line)
        : condition(move(condition)), body(move(body)) { this->line = line; }
};

struct ContinueStmt : Stmt {
    explicit ContinueStmt(int line) { this->line = line; }
};

struct BreakStmt : Stmt {
    explicit BreakStmt(int line) { this->line = line; }
};

struct Program {
    vector<StmtPtr> statements;
};

// ----------------------------- Parser ----------------------------
class Parser {
private:
    vector<Token> tokens;
    size_t current = 0;
    vector<string> &errors;

    bool isAtEnd() const { return peek().type == TokenType::END_OF_FILE; }
    const Token &peek() const { return tokens[current]; }
    const Token &previous() const { return tokens[current - 1]; }

    bool check(TokenType type) const { return !isAtEnd() && peek().type == type; }

    bool checkAny(initializer_list<TokenType> types) const {
        for (auto t : types) if (check(t)) return true;
        return false;
    }

    const Token &advance() {
        if (!isAtEnd()) ++current;
        return previous();
    }

    bool match(TokenType type) {
        if (!check(type)) return false;
        advance();
        return true;
    }

    void errorAt(const Token &tok, const string &msg) {
        errors.push_back("Parser error at line " + to_string(tok.line) +
                         ", column " + to_string(tok.column) + ": " + msg);
    }

    Token consume(TokenType type, const string &msg) {
        if (check(type)) return advance();
        errorAt(peek(), msg + ". Found '" +
                (peek().lexeme.empty() ? tokenTypeName(peek().type) : peek().lexeme) + "'");
        return Token(type, "", peek().line, peek().column);
    }

    void synchronize() {
        // Basic error recovery: skip to semicolon or a block boundary.
        while (!isAtEnd()) {
            if (previous().type == TokenType::SEMICOLON) return;
            if (checkAny({TokenType::KW_SHONKHA, TokenType::KW_DOSHOMIK,
                          TokenType::KW_SHOBDO, TokenType::KW_JODI,
                          TokenType::KW_JOKHON, TokenType::KW_LEHKHO,
                          TokenType::KW_CHOLO, TokenType::KW_THAMVAI,
                          TokenType::RBRACE})) return;
            advance();
        }
    }

    DataType parseType() {
        if (match(TokenType::KW_SHONKHA)) return DataType::INT;
        if (match(TokenType::KW_DOSHOMIK)) return DataType::DOUBLE;
        if (match(TokenType::KW_SHOBDO)) return DataType::STRING;

        errorAt(peek(), "Expected a data type (shonkha, doshomik, or shobdo)");
        return DataType::ERROR;
    }

    StmtPtr declaration() {
        int stmtLine = peek().line;
        DataType type = parseType();
        Token name = consume(TokenType::IDENTIFIER, "Expected variable name");
        ExprPtr initializer;
        if (match(TokenType::ASSIGN)) {
            initializer = expression();
        } else {
            // Default values are useful for a beginner-friendly language.
            if (type == DataType::INT)
                initializer = make_unique<LiteralExpr>(LiteralExpr::Kind::INT, "0");
            else if (type == DataType::DOUBLE)
                initializer = make_unique<LiteralExpr>(LiteralExpr::Kind::DOUBLE, "0.0");
            else
                initializer = make_unique<LiteralExpr>(LiteralExpr::Kind::STRING, "");
        }
        consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
        return make_unique<VarDeclStmt>(type, name.lexeme, move(initializer), stmtLine);
    }

    StmtPtr assignmentOrExprStatement() {
        int stmtLine = peek().line;

        if (check(TokenType::IDENTIFIER) && current + 1 < tokens.size()
            && tokens[current + 1].type == TokenType::ASSIGN) {
            Token name = advance();
            advance();
            ExprPtr value = expression();
            consume(TokenType::SEMICOLON, "Expected ';' after assignment");
            return make_unique<AssignmentStmt>(name.lexeme, move(value), stmtLine);
        }

        ExprPtr expr = expression();
        consume(TokenType::SEMICOLON, "Expected ';' after expression");
        return make_unique<ExprStmt>(move(expr), stmtLine);
    }

    StmtPtr printStatement() {
        int stmtLine = peek().line;
        advance(); // lehkho
        consume(TokenType::LPAREN, "Expected '(' after lehkho");
        ExprPtr expr = expression();
        consume(TokenType::RPAREN, "Expected ')' after lehkho argument");
        consume(TokenType::SEMICOLON, "Expected ';' after lehkho statement");
        return make_unique<PrintStmt>(move(expr), stmtLine);
    }

    unique_ptr<BlockStmt> block() {
        auto result = make_unique<BlockStmt>();
        consume(TokenType::LBRACE, "Expected '{' to begin block");

        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            try {
                result->statements.push_back(statement());
            } catch (...) {
                synchronize();
            }
        }

        consume(TokenType::RBRACE, "Expected '}' after block");
        return result;
    }

    StmtPtr ifStatement() {
        int stmtLine = peek().line;

        advance(); // jodi

        consume(TokenType::LPAREN, "Expected '(' after jodi");
        ExprPtr condition = expression();
        consume(TokenType::RPAREN, "Expected ')' after condition");

        auto thenBranch = block();

        unique_ptr<IfStmt> elseIf;
        unique_ptr<BlockStmt> elseBranch;

        if (match(TokenType::KW_NAHOLE)) {

            // `nahole jodi (...) { ... }`
            if (check(TokenType::KW_JODI)) {
                StmtPtr nested = ifStatement();
                elseIf.reset(static_cast<IfStmt*>(nested.release()));
            } else {
                // Plain `nahole { ... }`
                elseBranch = block();
            }
        }

        return make_unique<IfStmt>(
            move(condition),
            move(thenBranch),
            move(elseIf),
            move(elseBranch),
            stmtLine
        );
    }

    StmtPtr whileStatement() {
        int stmtLine = peek().line;
        advance(); // jokhon
        consume(TokenType::LPAREN, "Expected '(' after jokhon");
        ExprPtr condition = expression();
        consume(TokenType::RPAREN, "Expected ')' after condition");
        auto body = block();
        return make_unique<WhileStmt>(move(condition), move(body), stmtLine);
    }

    StmtPtr continueStatement() {
        int stmtLine = peek().line;
        advance(); // cholo
        consume(TokenType::SEMICOLON, "Expected ';' after cholo");
        return make_unique<ContinueStmt>(stmtLine);
    }

    StmtPtr breakStatement() {
        int stmtLine = peek().line;
        advance(); // thamvai
        consume(TokenType::SEMICOLON, "Expected ';' after thamvai");
        return make_unique<BreakStmt>(stmtLine);
    }

    StmtPtr statement() {
        if (checkAny({TokenType::KW_SHONKHA, TokenType::KW_DOSHOMIK, TokenType::KW_SHOBDO}))
            return declaration();
        if (check(TokenType::KW_LEHKHO)) return printStatement();
        if (check(TokenType::KW_JODI)) return ifStatement();
        if (check(TokenType::KW_JOKHON)) return whileStatement();
        if (check(TokenType::KW_CHOLO)) return continueStatement();
        if (check(TokenType::KW_THAMVAI)) return breakStatement();
        if (check(TokenType::RBRACE)) {
            errorAt(peek(), "Unexpected '}'");
            advance();
            return make_unique<ExprStmt>(make_unique<LiteralExpr>(LiteralExpr::Kind::INT, "0"), peek().line);
        }
        return assignmentOrExprStatement();
    }

    ExprPtr expression() { return logicalOr(); }

    // Precedence (low -> high):
    // || (lowest logical precedence)
    // &&
    // == !=
    // < <= > >=
    // + -
    // * / %
    // unary -
    // primary
    ExprPtr logicalOr() {
        ExprPtr expr = logicalAnd();
        while (match(TokenType::OR_OR)) {
            TokenType op = previous().type;
            ExprPtr right = logicalAnd();
            expr = make_unique<BinaryExpr>(move(expr), op, move(right));
        }
        return expr;
    }

    ExprPtr logicalAnd() {
        ExprPtr expr = equality();
        while (match(TokenType::AND_AND)) {
            TokenType op = previous().type;
            ExprPtr right = equality();
            expr = make_unique<BinaryExpr>(move(expr), op, move(right));
        }
        return expr;
    }

    ExprPtr equality() {
        ExprPtr expr = comparison();
        while (match(TokenType::EQUAL_EQUAL) || match(TokenType::NOT_EQUAL)) {
            TokenType op = previous().type;
            ExprPtr right = comparison();
            expr = make_unique<BinaryExpr>(move(expr), op, move(right));
        }
        return expr;
    }

    ExprPtr comparison() {
        ExprPtr expr = term();
        while (checkAny({TokenType::LESS, TokenType::LESS_EQUAL,
                          TokenType::GREATER, TokenType::GREATER_EQUAL})) {
            TokenType op = advance().type;
            ExprPtr right = term();
            expr = make_unique<BinaryExpr>(move(expr), op, move(right));
        }
        return expr;
    }

    ExprPtr term() {
        ExprPtr expr = factor();
        while (checkAny({TokenType::PLUS, TokenType::MINUS})) {
            TokenType op = advance().type;
            ExprPtr right = factor();
            expr = make_unique<BinaryExpr>(move(expr), op, move(right));
        }
        return expr;
    }

    ExprPtr factor() {
        ExprPtr expr = unary();
        while (checkAny({TokenType::STAR, TokenType::SLASH, TokenType::PERCENT})) {
            TokenType op = advance().type;
            ExprPtr right = unary();
            expr = make_unique<BinaryExpr>(move(expr), op, move(right));
        }
        return expr;
    }

    ExprPtr unary() {
        if (match(TokenType::MINUS)) {
            return make_unique<UnaryExpr>(TokenType::MINUS, unary());
        }
        return primary();
    }

    ExprPtr primary() {
        if (match(TokenType::INTEGER_LITERAL)) {
            return make_unique<LiteralExpr>(LiteralExpr::Kind::INT, previous().lexeme);
        }
        if (match(TokenType::FLOAT_LITERAL)) {
            return make_unique<LiteralExpr>(LiteralExpr::Kind::DOUBLE, previous().lexeme);
        }
        if (match(TokenType::STRING_LITERAL)) {
            return make_unique<LiteralExpr>(LiteralExpr::Kind::STRING, previous().lexeme);
        }
        if (match(TokenType::IDENTIFIER)) {
            return make_unique<VariableExpr>(previous().lexeme);
        }
        if (match(TokenType::KW_INFOBRO)) {
            consume(TokenType::LPAREN, "Expected '(' after infobro");
            consume(TokenType::RPAREN, "Expected ')' after infobro");
            return make_unique<InputExpr>();
        }
        if (match(TokenType::LPAREN)) {
            ExprPtr expr = expression();
            consume(TokenType::RPAREN, "Expected ')' after expression");
            return expr;
        }

        errorAt(peek(), "Expected expression");
        // Return a safe placeholder AST node so compilation can continue.
        if (!isAtEnd()) advance();
        return make_unique<LiteralExpr>(LiteralExpr::Kind::INT, "0");
    }

public:
    Parser(vector<Token> tokens, vector<string> &errors)
        : tokens(move(tokens)), errors(errors) {}

    Program parse() {
        Program program;
        while (!isAtEnd()) {
            try {
                program.statements.push_back(statement());
            } catch (...) {
                synchronize();
            }
        }
        return program;
    }
};

// -------------------------- Type Checker -------------------------
class TypeChecker {
private:
    unordered_map<string, DataType> symbols;
    vector<string> &errors;
    int loopDepth = 0;

    void error(int line, const string &msg) {
        errors.push_back("Type error at line " + to_string(line) + ": " + msg);
    }

    bool numeric(DataType t) const { return t == DataType::INT || t == DataType::DOUBLE; }

    bool assignable(DataType target, DataType source) const {
        if (target == source) return true;
        // Safe widening: integer -> double.
        if (target == DataType::DOUBLE && source == DataType::INT) return true;
        return false;
    }

    DataType checkExpr(Expr *expr, int line) {
        if (!expr) return DataType::ERROR;

        if (auto *e = dynamic_cast<LiteralExpr*>(expr)) {
            if (e->kind == LiteralExpr::Kind::INT) return expr->inferredType = DataType::INT;
            if (e->kind == LiteralExpr::Kind::DOUBLE) return expr->inferredType = DataType::DOUBLE;
            return expr->inferredType = DataType::STRING;
        }

        if (auto *e = dynamic_cast<VariableExpr*>(expr)) {
            auto it = symbols.find(e->name);
            if (it == symbols.end()) {
                error(line, "Variable '" + e->name + "' was not declared");
                return expr->inferredType = DataType::ERROR;
            }
            return expr->inferredType = it->second;
        }

        if (dynamic_cast<InputExpr*>(expr)) {
            // Type is resolved from assignment context during generation.
            return expr->inferredType = DataType::ERROR;
        }

        if (auto *e = dynamic_cast<UnaryExpr*>(expr)) {
            DataType t = checkExpr(e->right.get(), line);
            if (t != DataType::ERROR && !numeric(t)) {
                error(line, "Unary '-' requires a numeric operand");
                return expr->inferredType = DataType::ERROR;
            }
            return expr->inferredType = t;
        }

        if (auto *e = dynamic_cast<BinaryExpr*>(expr)) {
            DataType left = checkExpr(e->left.get(), line);
            DataType right = checkExpr(e->right.get(), line);

            if (left == DataType::ERROR || right == DataType::ERROR) {
                return expr->inferredType = DataType::ERROR;
            }

            bool logical = (e->op == TokenType::AND_AND || e->op == TokenType::OR_OR);
            if (logical) {
                if (left == DataType::BOOL && right == DataType::BOOL) {
                    return expr->inferredType = DataType::BOOL;
                }
                error(line, "Logical operators '&&' and '||' require boolean operands");
                return expr->inferredType = DataType::ERROR;
            }

            bool comparison = (e->op == TokenType::EQUAL_EQUAL || e->op == TokenType::NOT_EQUAL ||
                               e->op == TokenType::LESS || e->op == TokenType::LESS_EQUAL ||
                               e->op == TokenType::GREATER || e->op == TokenType::GREATER_EQUAL);

            if (comparison) {
                if ((numeric(left) && numeric(right)) || left == right) {
                    return expr->inferredType = DataType::BOOL;
                }
                error(line, "Cannot compare " + dataTypeName(left) + " with " + dataTypeName(right));
                return expr->inferredType = DataType::ERROR;
            }

            if (e->op == TokenType::PLUS && left == DataType::STRING && right == DataType::STRING) {
                return expr->inferredType = DataType::STRING;
            }

            if (!numeric(left) || !numeric(right)) {
                error(line, "Arithmetic operators require numeric operands");
                return expr->inferredType = DataType::ERROR;
            }

            if (e->op == TokenType::PERCENT && (left != DataType::INT || right != DataType::INT)) {
                error(line, "'%' requires integer operands");
                return expr->inferredType = DataType::ERROR;
            }

            if (left == DataType::DOUBLE || right == DataType::DOUBLE)
                return expr->inferredType = DataType::DOUBLE;
            return expr->inferredType = DataType::INT;
        }

        return DataType::ERROR;
    }

    void checkStatement(Stmt *stmt) {
        if (!stmt) return;

        if (auto *s = dynamic_cast<VarDeclStmt*>(stmt)) {
            if (symbols.count(s->name)) {
                error(s->line, "Variable '" + s->name + "' is already declared");
                return;
            }
            DataType actual;
            if (dynamic_cast<InputExpr*>(s->initializer.get())) {
                actual = s->type; // input is context-sensitive
            } else {
                actual = checkExpr(s->initializer.get(), s->line);
            }
            if (actual != DataType::ERROR && !assignable(s->type, actual)) {
                error(s->line, "Cannot assign " + dataTypeName(actual) +
                      " to " + dataTypeName(s->type));
            }
            symbols[s->name] = s->type;
            return;
        }

        if (auto *s = dynamic_cast<AssignmentStmt*>(stmt)) {
            auto it = symbols.find(s->name);
            if (it == symbols.end()) {
                error(s->line, "Variable '" + s->name + "' was not declared");
                return;
            }
            DataType actual = dynamic_cast<InputExpr*>(s->value.get())
                                ? it->second
                                : checkExpr(s->value.get(), s->line);
            if (actual != DataType::ERROR && !assignable(it->second, actual)) {
                error(s->line, "Cannot assign " + dataTypeName(actual) +
                      " to " + dataTypeName(it->second) + " variable '" + s->name + "'");
            }
            return;
        }

        if (auto *s = dynamic_cast<PrintStmt*>(stmt)) {
            DataType t = checkExpr(s->value.get(), s->line);
            // Input has no standalone fixed type, but it is valid in print.
            if (dynamic_cast<InputExpr*>(s->value.get())) return;
            (void)t;
            return;
        }

        if (auto *s = dynamic_cast<ExprStmt*>(stmt)) {
            checkExpr(s->value.get(), s->line);
            return;
        }

        if (auto *s = dynamic_cast<IfStmt*>(stmt)) {
            DataType t = checkExpr(s->condition.get(), s->line);
            if (t != DataType::BOOL && t != DataType::ERROR) {
                error(s->line, "jodi condition must be a boolean expression");
            }
            // Same scope model as a simple language: declarations inside blocks are visible
            // afterwards. This keeps the implementation simple for a course project.
            for (auto &child : s->thenBranch->statements) checkStatement(child.get());

            if (s->elseIf) {
                // An else-if is another if statement, but it is still part
                // of the same surrounding control-flow construct.
                checkStatement(s->elseIf.get());
            }

            if (s->elseBranch) {
                for (auto &child : s->elseBranch->statements) {
                    checkStatement(child.get());
                }
            }

            return;
        }

        if (auto *s = dynamic_cast<WhileStmt*>(stmt)) {
            DataType t = checkExpr(s->condition.get(), s->line);
            if (t != DataType::BOOL && t != DataType::ERROR) {
                error(s->line, "jokhon condition must be a boolean expression");
            }

            ++loopDepth;
            for (auto &child : s->body->statements) checkStatement(child.get());
            --loopDepth;
            return;
        }

        if (dynamic_cast<ContinueStmt*>(stmt)) {
            if (loopDepth == 0) {
                error(stmt->line, "'cholo' can only be used inside a jokhon loop");
            }
            return;
        }

        if (dynamic_cast<BreakStmt*>(stmt)) {
            if (loopDepth == 0) {
                error(stmt->line, "'thamvai' can only be used inside a jokhon loop");
            }
            return;
        }
    }

public:
    explicit TypeChecker(vector<string> &errors) : errors(errors) {}

    bool check(const Program &program) {
        for (const auto &stmt : program.statements) checkStatement(stmt.get());
        return true;
    }
};

// ------------------------ Python Code Generator ------------------
class PythonGenerator {
private:
    int indent = 0;
    ostringstream out;
    unordered_map<string, DataType> types;

    string spaces() const { return string(indent * 4, ' '); }

    string quote(const string &s) {
        string result = "\"";
        for (char c : s) {
            if (c == '\\') result += "\\\\";
            else if (c == '"') result += "\\\"";
            else if (c == '\n') result += "\\n";
            else if (c == '\t') result += "\\t";
            else result += c;
        }
        result += "\"";
        return result;
    }

    string expr(const Expr *e, optional<DataType> expected = nullopt) {
        if (!e) return "0";

        if (auto *x = dynamic_cast<const LiteralExpr*>(e)) {
            if (x->kind == LiteralExpr::Kind::STRING) return quote(x->value);
            return x->value;
        }
        if (auto *x = dynamic_cast<const VariableExpr*>(e)) return x->name;
        if (dynamic_cast<const InputExpr*>(e)) {
            if (expected.has_value()) {
                if (*expected == DataType::INT) return "int(input())";
                if (*expected == DataType::DOUBLE) return "float(input())";
            }
            return "input()";
        }
        if (auto *x = dynamic_cast<const UnaryExpr*>(e)) {
            return "(-" + expr(x->right.get()) + ")";
        }
        if (auto *x = dynamic_cast<const BinaryExpr*>(e)) {
            string op;
            switch (x->op) {
                case TokenType::PLUS: op = "+"; break;
                case TokenType::MINUS: op = "-"; break;
                case TokenType::STAR: op = "*"; break;
                case TokenType::SLASH: op = "/"; break;
                case TokenType::PERCENT: op = "%"; break;
                case TokenType::EQUAL_EQUAL: op = "=="; break;
                case TokenType::NOT_EQUAL: op = "!="; break;
                case TokenType::LESS: op = "<"; break;
                case TokenType::LESS_EQUAL: op = "<="; break;
                case TokenType::GREATER: op = ">"; break;
                case TokenType::GREATER_EQUAL: op = ">="; break;
                case TokenType::AND_AND: op = "and"; break;
                case TokenType::OR_OR: op = "or"; break;
                default: op = "+"; break;
            }
            return "(" + expr(x->left.get()) + " " + op + " " + expr(x->right.get()) + ")";
        }
        return "0";
    }

    void emitBlock(const BlockStmt *block) {
        ++indent;
        for (const auto &stmt : block->statements) emitStmt(stmt.get());
        --indent;
    }

    void emitElseIf(const IfStmt *s) {
        out << spaces() << "elif " << expr(s->condition.get()) << ":\n";

        if (s->thenBranch->statements.empty()) {
            ++indent;
            out << spaces() << "pass\n";
            --indent;
        } else {
            emitBlock(s->thenBranch.get());
        }

        if (s->elseIf) {
            emitElseIf(s->elseIf.get());
        } else if (s->elseBranch) {
            out << spaces() << "else:\n";

            if (s->elseBranch->statements.empty()) {
                ++indent;
                out << spaces() << "pass\n";
                --indent;
            } else {
                emitBlock(s->elseBranch.get());
            }
        }
    }

    void emitStmt(const Stmt *stmt) {
        if (auto *s = dynamic_cast<const VarDeclStmt*>(stmt)) {
            types[s->name] = s->type;
            out << spaces() << s->name << " = " << expr(s->initializer.get(), s->type) << "\n";
            return;
        }
        if (auto *s = dynamic_cast<const AssignmentStmt*>(stmt)) {
            DataType t = types.count(s->name) ? types[s->name] : DataType::ERROR;
            out << spaces() << s->name << " = " << expr(s->value.get(), t) << "\n";
            return;
        }
        if (auto *s = dynamic_cast<const PrintStmt*>(stmt)) {
            out << spaces() << "print(" << expr(s->value.get()) << ")\n";
            return;
        }
        if (auto *s = dynamic_cast<const ExprStmt*>(stmt)) {
            out << spaces() << expr(s->value.get()) << "\n";
            return;
        }
        if (auto *s = dynamic_cast<const IfStmt*>(stmt)) {
            out << spaces() << "if " << expr(s->condition.get()) << ":\n";
            if (s->thenBranch->statements.empty()) {
                ++indent;
                out << spaces() << "pass\n";
                --indent;
            } else {
                emitBlock(s->thenBranch.get());
            }
            if (s->elseIf) {
                emitElseIf(s->elseIf.get());
            } else if (s->elseBranch) {
                out << spaces() << "else:\n";

                if (s->elseBranch->statements.empty()) {
                    ++indent;
                    out << spaces() << "pass\n";
                    --indent;
                } else {
                    emitBlock(s->elseBranch.get());
                }
            }

            return;
        }
        if (auto *s = dynamic_cast<const WhileStmt*>(stmt)) {
            out << spaces() << "while " << expr(s->condition.get()) << ":\n";
            if (s->body->statements.empty()) {
                ++indent;
                out << spaces() << "pass\n";
                --indent;
            } else {
                emitBlock(s->body.get());
            }
            return;
        }
        if (dynamic_cast<const ContinueStmt*>(stmt)) {
            out << spaces() << "continue\n";
            return;
        }
        if (dynamic_cast<const BreakStmt*>(stmt)) {
            out << spaces() << "break\n";
            return;
        }
    }

public:
    string generate(const Program &program) {
        out.str("");
        out.clear();
        types.clear();
        indent = 0;
        out << "# Generated by BangLish++ Compiler\n";
        out << "# Target language: Python 3\n\n";
        for (const auto &stmt : program.statements) emitStmt(stmt.get());
        return out.str();
    }
};

// ------------------------------ Utils ----------------------------
static string readFile(const string &path) {
    ifstream file(path);
    if (!file) throw runtime_error("Cannot open source file: " + path);
    return string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
}

static bool writeFile(const string &path, const string &content) {
    ofstream file(path);
    if (!file) return false;
    file << content;
    return true;
}

static void printTokens(const vector<Token> &tokens) {
    cout << "\n=== TOKENS ===\n";
    for (const auto &t : tokens) {
        cout << "Line " << t.line << ", Col " << t.column
             << " | " << tokenTypeName(t.type)
             << " | '" << t.lexeme << "'\n";
    }
}

int main(int argc, char **argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (argc < 2 || argc > 3) {
        cerr << "Usage: " << argv[0] << " <source.bpp> [output.py]\n";
        cerr << "Example: " << argv[0] << " examples/demo.bpp demo.py\n";
        return 1;
    }

    string sourcePath = argv[1];
    string outputPath = (argc == 3) ? argv[2] : "generated.py";

    string source;
    try {
        source = readFile(sourcePath);
    } catch (const exception &e) {
        cerr << e.what() << '\n';
        return 1;
    }

    vector<string> errors;

    // 1. Lexical Analysis
    Lexer lexer(source, errors);
    vector<Token> tokens = lexer.tokenize();

    if (!errors.empty()) {
        cerr << "\n=== ERRORS ===\n";
        for (const auto &e : errors) cerr << e << '\n';
        return 1;
    }

    // 2. Syntax Analysis / Parsing
    Parser parser(tokens, errors);
    Program program = parser.parse();

    if (!errors.empty()) {
        cerr << "\n=== ERRORS ===\n";
        for (const auto &e : errors) cerr << e << '\n';
        return 1;
    }

    // 3. Semantic Analysis / Type Checking
    TypeChecker checker(errors);
    checker.check(program);

    if (!errors.empty()) {
        cerr << "\n=== ERRORS ===\n";
        for (const auto &e : errors) cerr << e << '\n';
        return 1;
    }

    // 4. Target Code Generation
    PythonGenerator generator;
    string pythonCode = generator.generate(program);

    if (!writeFile(outputPath, pythonCode)) {
        cerr << "Cannot write generated target file: " << outputPath << '\n';
        return 1;
    }

    cout << "Compilation successful.\n";
    cout << "Generated Python file: " << outputPath << '\n';
    return 0;
}
