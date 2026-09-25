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

            // Python's '/' operator produces a floating-point result, so the
            // Banglish++ type system treats division as doshomik as well.
            if (e->op == TokenType::SLASH)
                return expr->inferredType = DataType::DOUBLE;

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
