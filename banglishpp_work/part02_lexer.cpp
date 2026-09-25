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
