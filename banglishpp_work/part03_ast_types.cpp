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
    LiteralExpr(Kind kind, string value) : kind(kind), value(std::move(value)) {}
};

struct VariableExpr : Expr {
    string name;
    explicit VariableExpr(string name) : name(std::move(name)) {}
};

struct InputExpr : Expr {
    InputExpr() = default;
};

struct UnaryExpr : Expr {
    TokenType op;
    ExprPtr right;
    UnaryExpr(TokenType op, ExprPtr right) : op(op), right(std::move(right)) {}
};

struct BinaryExpr : Expr {
    ExprPtr left;
    TokenType op;
    ExprPtr right;
    BinaryExpr(ExprPtr left, TokenType op, ExprPtr right)
        : left(std::move(left)), op(op), right(std::move(right)) {}
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
        : type(type), name(std::move(name)), initializer(std::move(initializer)) { this->line = line; }
};

struct AssignmentStmt : Stmt {
    string name;
    ExprPtr value;
    AssignmentStmt(string name, ExprPtr value, int line)
        : name(std::move(name)), value(std::move(value)) { this->line = line; }
};

struct PrintStmt : Stmt {
    ExprPtr value;
    PrintStmt(ExprPtr value, int line) : value(std::move(value)) { this->line = line; }
};

struct ExprStmt : Stmt {
    ExprPtr value;
    ExprStmt(ExprPtr value, int line) : value(std::move(value)) { this->line = line; }
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
        : condition(std::move(condition)),
          thenBranch(std::move(thenBranch)),
          elseIf(std::move(elseIf)),
          elseBranch(std::move(elseBranch)) {
        this->line = line;
    }
};

struct WhileStmt : Stmt {
    ExprPtr condition;
    unique_ptr<BlockStmt> body;
    WhileStmt(ExprPtr condition, unique_ptr<BlockStmt> body, int line)
        : condition(std::move(condition)), body(std::move(body)) { this->line = line; }
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
