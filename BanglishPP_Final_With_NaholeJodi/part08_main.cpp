// ------------------------------ Main -----------------------------
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
