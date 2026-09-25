#include <bits/stdc++.h>
using namespace std;

#include "part01_token.hpp"
#include "part03_ast_types.cpp"
#include "part04_parser.cpp"
#include "part05_type_checker.cpp"
#include "part06_python_code_generator.cpp"
#include "part07_utils.cpp"

vector<Token> lexWithFlex(const string &source, vector<string> &errors);

// ------------------------------ Main -----------------------------
int main(int argc, char **argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (argc < 2 || argc > 4) {
        cerr << "Usage: " << argv[0] << " <source.bpp> [output.py] [--no-run]\n";
        cerr << "Example: " << argv[0] << " nahole_jodi_demo.bpp generated.py\n";
        return 1;
    }

    const string sourcePath = argv[1];
    string outputPath = "generated.py";
    bool runProgram = true;

    for (int i = 2; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--no-run") {
            runProgram = false;
        } else if (outputPath == "generated.py") {
            outputPath = arg;
        } else {
            cerr << "Unknown or duplicate argument: " << arg << '\n';
            return 1;
        }
    }

    string source;
    try {
        source = readFile(sourcePath);
    } catch (const exception &e) {
        cerr << e.what() << '\n';
        return 1;
    }

    vector<string> errors;

    // 1. Lexical Analysis
    vector<Token> tokens = lexWithFlex(source, errors);

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
    const string pythonCode = generator.generate(program);

    if (!writeFile(outputPath, pythonCode)) {
        cerr << "Cannot write generated target file: " << outputPath << '\n';
        return 1;
    }

    cout << "Compilation successful.\n";
    cout << "Generated Python file: " << outputPath << '\n';

    if (!runProgram) return 0;

    cout << "\n=== PROGRAM OUTPUT ===\n";
    cout.flush();
    const int status = runPythonProgram(outputPath);
    if (status != 0) {
        cerr << "\nProgram execution failed (python3 exited with a non-zero status).\n";
        return 1;
    }

    return 0;
}
