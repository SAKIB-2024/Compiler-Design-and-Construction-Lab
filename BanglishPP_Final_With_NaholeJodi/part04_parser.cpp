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
