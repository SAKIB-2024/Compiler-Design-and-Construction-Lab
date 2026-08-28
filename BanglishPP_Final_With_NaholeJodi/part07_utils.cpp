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
