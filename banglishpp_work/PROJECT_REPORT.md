# Banglish++ Compiler Project Report

## 1. Project Overview

Banglish++ is a small educational compiler implemented in C++17. It translates Banglish++ source programs into executable Python 3 source code.

Compiler pipeline:

`Banglish++ source -> Flex/Lex lexer -> tokens -> Recursive Descent parser -> AST -> type checker -> Python code generator`

## 2. Required Tools and Implementation

| Component | Requirement | Implementation |
|---|---|---|
| Lexer | Lex/Flex | `banglishpp_lexer.l` |
| Parser | LL(0)/LR(0)/LR(1)/Recursive Descent/Bison/Yacc | Recursive Descent (`part04_parser.cpp`) |
| Implementation language | C++ / Java | C++17 |
| Target language | Java or Python | Python 3 |

The repository also retains `banglishpp_all_parts.cpp` as a standalone fallback build. The rubric-compliant build uses the Flex specification and `Makefile`.

## 3. Minimum Acceptable Features Checklist

### 3.1 Two data types with type checking

Implemented. Banglish++ supports three user-visible data types:

- `shonkha` -> integer
- `doshomik` -> floating point
- `shobdo` -> string

The type checker detects undeclared variables, incompatible assignments, invalid arithmetic operands, invalid comparison operands, and invalid logical operands.

### 3.2 Basic arithmetic with correct precedence

Implemented using recursive-descent precedence levels:

1. `||`
2. `&&`
3. `==`, `!=`
4. `<`, `<=`, `>`, `>=`
5. `+`, `-`
6. `*`, `/`, `%`
7. unary `-`
8. primary expressions

Therefore `2 + 3 * 4` is parsed as `2 + (3 * 4)`.

Division is typed as `doshomik`, matching Python `/` semantics.

### 3.3 Assignment statements

Implemented:

```text
shonkha x = 10;
x = x + 5;
```

Assignments are type checked against the declared variable type.

### 3.4 IF-ELSE conditional control

Implemented with:

```text
jodi (...) { ... }
nhole { ... }
```

and extended support for `nahole jodi (...) { ... }`.

The Python generator emits `if`, `elif`, and `else`.

### 3.5 WHILE looping control

Implemented with:

```text
jokhon (...) {
    ...
}
```

The project also supports `cholo` (continue) and `thamvai` (break), restricted to `jokhon` loops by semantic checking.

### 3.6 Basic syntax error recovery

Implemented in the parser. Syntax errors now throw an internal `ParseError`, and the parser synchronizes by skipping input until a semicolon, the next source line, or a block boundary.

This is a real recovery path; the previous version recorded errors but did not actually enter its `catch` blocks.

### 3.7 No compiler runtime crashes

The compiler uses `unique_ptr` for AST ownership, checks tokens before advancing, and uses safe placeholder handling through structured parser errors. Malformed input is rejected with diagnostics instead of being dereferenced through null AST pointers.

The test suite includes malformed expressions, missing semicolons, missing braces, multiple syntax errors, illegal loop-control statements, and type errors.

### 3.8 Valid executable Python target code

Implemented. The compiler writes a `.py` file and runs it with Python 3 by default. Use `--no-run` for compile-only mode. The regression suite also syntax-checks successful generated Python and verifies runtime output for the main demo.

Additional hardening was added for target-code correctness:

- Every source identifier is emitted as `bpp_<name>`, preventing collisions with Python keywords or built-in names.
- Integer literals are normalized so values such as `00042` become valid Python `42`.
- Floating-point literals are normalized similarly.
- Generated expressions use Python-compatible arithmetic and logical operators.

## 4. Important Changes Made During the Final Audit

### Change 1 — Real parser error recovery

**File:** `part04_parser.cpp`

Added `ParseError`, `failAt()`, and line/semicolon/block synchronization. The old implementation had `try/catch` blocks, but parsing functions only recorded errors and did not throw, so recovery was not actually active.

### Change 2 — Python-safe identifier generation

**File:** `part06_python_code_generator.cpp`

Added `pyName()` and changed variable declarations, assignments, and variable expressions to use `bpp_<source_name>`.

Example:

```text
shonkha class = 1;
```

now generates:

```python
bpp_class = 1
```

### Change 3 — Python-safe numeric literals

**File:** `part06_python_code_generator.cpp`

Leading zeroes are removed from integer and floating-point literals before Python code generation.

### Change 4 — Correct division type

**File:** `part05_type_checker.cpp`

The `/` operator now returns `doshomik`, matching Python's floating-point division.

### Change 5 — Flex/Lex lexer implementation

**Files:** `banglishpp_lexer.l`, `part01_token.hpp`, `banglishpp_flex_main.cpp`, `Makefile`

A real Flex lexer specification was added and connected to the existing parser, type checker, and Python generator. The required build is:

```bash
make
```

which runs Flex and then compiles the generated scanner with the C++ compiler.

## 5. Testing

The repository contains regression tests in `tests/` covering:

- arithmetic precedence
- integer/double/string typing
- assignments
- if/else
- while
- continue/break
- Python-keyword identifiers
- leading-zero numeric literals
- valid and invalid division
- malformed expressions
- missing semicolons
- missing braces
- multiple syntax errors
- invalid loop-control placement
- empty source files

Run the source-only regression suite with:

```bash
make test
```

Successful target files are additionally checked using:

```bash
python3 -m py_compile generated.py
```

## 6. Conclusion

After the final audit, the project satisfies the requested minimum language features, uses Recursive Descent parsing, provides a Flex/Lex lexer specification, performs semantic type checking, recovers from basic syntax errors, and generates Python 3 target code that is syntax-validated by the regression tests.
