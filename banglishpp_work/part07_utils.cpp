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

static string shellQuote(const string &value) {
    string result = "'";
    for (char c : value) {
        if (c == '\'') result += "'\\''";
        else result += c;
    }
    result += "'";
    return result;
}

static int runPythonProgram(const string &path) {
    const string command = "python3 " + shellQuote(path);
    return system(command.c_str());
}
