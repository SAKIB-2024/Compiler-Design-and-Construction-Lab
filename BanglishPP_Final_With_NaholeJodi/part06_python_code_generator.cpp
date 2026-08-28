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
