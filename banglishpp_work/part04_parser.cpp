// ----------------------------- Parser ----------------------------
class Parser {
private:
    struct ParseError {
        int line;
    };

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
        failAt(peek(), msg + ". Found '" +
               (peek().lexeme.empty() ? tokenTypeName(peek().type) : peek().lexeme) + "'");
    }

    [[noreturn]] void failAt(const Token &tok, const string &msg) {
        errorAt(tok, msg);
        throw ParseError{tok.line};
    }

    void synchronize(int errorLine, bool stopAtRBrace) {
        // Basic error recovery: skip to a semicolon, the next source line,
        // or a block boundary.
        while (!isAtEnd()) {
            if (check(TokenType::SEMICOLON)) {
                advance();
                return;
            }
            if (stopAtRBrace && check(TokenType::RBRACE)) return;
            if (!stopAtRBrace && check(TokenType::RBRACE)) {
                advance();
                return;
            }
            if (peek().line > errorLine) return;
            advance();
        }
    }

    DataType parseType() {
        if (match(TokenType::KW_SHONKHA)) return DataType::INT;
        if (match(TokenType::KW_DOSHOMIK)) return DataType::DOUBLE;
        if (match(TokenType::KW_SHOBDO)) return DataType::STRING;

        failAt(peek(), "Expected a data type (shonkha, doshomik, or shobdo)");
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
        return make_unique<VarDeclStmt>(type, name.lexeme, std::move(initializer), stmtLine);
    }

    StmtPtr assignmentOrExprStatement() {
        int stmtLine = peek().line;

        if (check(TokenType::IDENTIFIER) && current + 1 < tokens.size()
            && tokens[current + 1].type == TokenType::ASSIGN) {
            Token name = advance();
            advance();
            ExprPtr value = expression();
            consume(TokenType::SEMICOLON, "Expected ';' after assignment");
            return make_unique<AssignmentStmt>(name.lexeme, std::move(value), stmtLine);
        }

        ExprPtr expr = expression();
        consume(TokenType::SEMICOLON, "Expected ';' after expression");
        return make_unique<ExprStmt>(std::move(expr), stmtLine);
    }

    StmtPtr printStatement() {
        int stmtLine = peek().line;
        advance(); // lehkho
        consume(TokenType::LPAREN, "Expected '(' after lehkho");
        ExprPtr expr = expression();
        consume(TokenType::RPAREN, "Expected ')' after lehkho argument");
        consume(TokenType::SEMICOLON, "Expected ';' after lehkho statement");
        return make_unique<PrintStmt>(std::move(expr), stmtLine);
    }

    unique_ptr<BlockStmt> block() {
        auto result = make_unique<BlockStmt>();
        consume(TokenType::LBRACE, "Expected '{' to begin block");

        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            try {
                result->statements.push_back(statement());
            } catch (const ParseError &e) {
                synchronize(e.line, true);
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
            std::move(condition),
            std::move(thenBranch),
            std::move(elseIf),
            std::move(elseBranch),
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
        return make_unique<WhileStmt>(std::move(condition), std::move(body), stmtLine);
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
            failAt(peek(), "Unexpected '}'");
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
            expr = make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    ExprPtr logicalAnd() {
        ExprPtr expr = equality();
        while (match(TokenType::AND_AND)) {
            TokenType op = previous().type;
            ExprPtr right = equality();
            expr = make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    ExprPtr equality() {
        ExprPtr expr = comparison();
        while (match(TokenType::EQUAL_EQUAL) || match(TokenType::NOT_EQUAL)) {
            TokenType op = previous().type;
            ExprPtr right = comparison();
            expr = make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    ExprPtr comparison() {
        ExprPtr expr = term();
        while (checkAny({TokenType::LESS, TokenType::LESS_EQUAL,
                          TokenType::GREATER, TokenType::GREATER_EQUAL})) {
            TokenType op = advance().type;
            ExprPtr right = term();
            expr = make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    ExprPtr term() {
        ExprPtr expr = factor();
        while (checkAny({TokenType::PLUS, TokenType::MINUS})) {
            TokenType op = advance().type;
            ExprPtr right = factor();
            expr = make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    ExprPtr factor() {
        ExprPtr expr = unary();
        while (checkAny({TokenType::STAR, TokenType::SLASH, TokenType::PERCENT})) {
            TokenType op = advance().type;
            ExprPtr right = unary();
            expr = make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
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

        failAt(peek(), "Expected expression");
    }

public:
    Parser(vector<Token> tokens, vector<string> &errors)
        : tokens(std::move(tokens)), errors(errors) {}

    Program parse() {
        Program program;
        while (!isAtEnd()) {
            try {
                program.statements.push_back(statement());
            } catch (const ParseError &e) {
                synchronize(e.line, false);
            }
        }
        return program;
    }
};
