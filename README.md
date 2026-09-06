# Banglish++

### A Bangla-inspired Programming Language and Compiler

Banglish++ is an educational programming language and compiler developed using **C++17** as part of a Compiler Design and Construction project.

The language uses Banglish-inspired keywords such as `shonkha`, `doshomik`, `shobdo`, `jodi`, `nahole`, and `jokhon`, making programming syntax more familiar and accessible to Bengali-speaking learners.

The compiler takes **Banglish++ (`.bpp`) source code** as input, performs lexical analysis, syntax analysis, semantic/type checking, and finally generates executable **Python 3** code.

---

## 👥 Team Members

| Name                     | Student ID       |
| ------------------------ | ---------------- |
| **Mahmudul Hasan Sakib** | 0182320012101417 |
| **Jasmin Bagom**         | 0182320012101409 |
| **Sakil Ahmed**          | 0182320012101403 |

---

## 🎯 Project Objective

The main objective of Banglish++ is to demonstrate the fundamental stages of compiler construction by building a small but functional compiler.

### Compiler Pipeline

```text
Banglish++ Source Code
        │
        ▼
   Lexical Analysis
     (Flex/Lex)
        │
        ▼
      Tokens
        │
        ▼
 Syntax Analysis
 (Recursive Descent Parser)
        │
        ▼
       AST
        │
        ▼
 Semantic Analysis
    (Type Checker)
        │
        ▼
 Python Code Generation
        │
        ▼
   Python 3 Program
```

---

## ✨ Features

### Data Types

Banglish++ currently supports three user-visible data types:

| Banglish++ | Meaning               |
| ---------- | --------------------- |
| `shonkha`  | Integer               |
| `doshomik` | Floating-point number |
| `shobdo`   | String                |

Example:

```text
shonkha age = 25;
doshomik height = 5.8;
shobdo name = "Sakib";
```

---

### Arithmetic Operators

The compiler supports:

```text
+
-
*
/
%
```

Operator precedence is handled by the recursive-descent parser.

For example:

```text
shonkha result = 2 + 3 * 4;
```

is interpreted as:

```text
2 + (3 * 4)
```

The supported precedence hierarchy includes:

```text
||
&&
== !=
< <= > >=
+ -
* / %
unary -
primary expressions
```

---

### Assignment

Variable declaration and assignment are supported.

```text
shonkha x = 10;

x = x + 5;
```

Assignments are checked against the declared variable type.

---

### Conditional Statements

Banglish++ supports `if`, `else if`, and `else` using Banglish-inspired keywords.

```text
jodi (x > 90) {
    lehkho("Excellent");
} nahole jodi (x > 70) {
    lehkho("Good");
} nahole {
    lehkho("Needs Improvement");
}
```

These are translated into Python:

```python
if ...
elif ...
else ...
```

---

### While Loop

The `jokhon` keyword is used for loops.

```text
jokhon (x < 10) {
    x = x + 1;
}
```

It is translated into a Python `while` loop.

---

### Loop Control

Banglish++ supports:

```text
cholo
thamvai
```

which correspond to:

```text
continue
break
```

These statements are semantically restricted to `jokhon` loops.

---

### Input and Output

The language provides Banglish-inspired I/O keywords including:

```text
infobro()
lehkho()
```

These are handled by the compiler's parsing and code-generation stages.

---

## 🔍 Lexical Analysis

Banglish++ uses **Flex/Lex** for lexical analysis.

The main lexer specification is:

```text
banglishpp_lexer.l
```

The lexer recognizes:

* Keywords
* Identifiers
* Integer literals
* Floating-point literals
* String literals
* Arithmetic operators
* Comparison operators
* Logical operators
* Assignment operator
* Parentheses
* Braces
* Semicolons
* Comments

Example:

```text
shonkha x = 10;
```

is converted into a sequence of tokens before being passed to the parser.

---

## 🧠 Syntax Analysis

The parser is implemented using a **Recursive Descent Parser**.

Main parser source:

```text
part04_parser.cpp
```

The parser constructs an **Abstract Syntax Tree (AST)** representing the structure of the source program.

The parser also includes basic syntax error recovery using synchronization at:

* Semicolons
* Source lines
* Block boundaries

This allows malformed programs to be rejected with useful diagnostics rather than causing compiler crashes.

---

## 🔬 Semantic Analysis & Type Checking

Semantic analysis is implemented in:

```text
part05_type_checker.cpp
```

The type checker validates:

* Variable declarations
* Undeclared variables
* Assignments
* Arithmetic expressions
* Comparison expressions
* Logical expressions
* Conditional expressions
* Type compatibility

For example, an invalid assignment such as:

```text
shonkha x = "hello";
```

is rejected during semantic analysis.

---

## ⚙️ Code Generation

The compiler generates **Python 3** code from the AST.

Main generator:

```text
part06_python_code_generator.cpp
```

For example, Banglish++:

```text
shonkha x = 10;

jodi (x > 5) {
    lehkho("Greater");
}
```

is translated into Python code with equivalent behavior.

### Python-safe Identifiers

To prevent conflicts with Python keywords and built-in names, source variables are generated using the prefix:

```text
bpp_
```

For example:

```text
shonkha class = 1;
```

becomes:

```python
bpp_class = 1
```

The compiler also normalizes numeric literals so values such as:

```text
00042
```

can be safely generated as:

```python
42
```

---

## 📁 Project Structure

```text
BanglishPP/
│
├── banglishpp_lexer.l
├── banglishpp_flex_main.cpp
│
├── part01_token.hpp
├── part01_token.cpp
├── part02_lexer.cpp
├── part03_ast_types.cpp
├── part04_parser.cpp
├── part05_type_checker.cpp
├── part06_python_code_generator.cpp
├── part07_utils.cpp
├── part08_main.cpp
│
├── banglishpp_all_parts.cpp
│
├── Makefile
│
├── tests/
│   ├── valid.bpp
│   ├── valid_types.bpp
│   ├── type_errors.bpp
│   ├── bad_expr.bpp
│   ├── missing_brace.bpp
│   ├── missing_semicolon.bpp
│   ├── multi_errors.bpp
│   ├── division_ok.bpp
│   ├── division_type_error.bpp
│   ├── leading_zero.bpp
│   ├── python_keyword.bpp
│   ├── invalid_loop_control.bpp
│   ├── empty.bpp
│   └── run_tests.sh
│
├── PROJECT_REPORT.md
├── README_PARTS.md
└── BanglishPP_PartByPart_Cpp.ipynb
```

---

## 🛠️ Technologies Used

| Technology                   | Purpose                 |
| ---------------------------- | ----------------------- |
| **C++17**                    | Compiler implementation |
| **Flex/Lex**                 | Lexical analysis        |
| **Recursive Descent Parser** | Syntax analysis         |
| **AST**                      | Program representation  |
| **Semantic Analysis**        | Type checking           |
| **Python 3**                 | Target language         |
| **Make**                     | Build automation        |
| **Bash**                     | Regression testing      |

---

## 🚀 Installation & Requirements

Before building the compiler, make sure the following are installed:

* C++17 compiler (`g++`)
* Flex
* Python 3
* Make

### Clone the repository

```bash
git clone https://github.com/your-username/BanglishPP.git
cd BanglishPP
```

---

## 🔨 Build the Compiler

The main build uses the **Flex lexer**:

```bash
make
```

This generates the Flex scanner and compiles the compiler.

Equivalent commands:

```bash
flex -o lex.yy.cpp banglishpp_lexer.l

g++ -std=c++17 -O2 -Wall -Wextra -pedantic \
    lex.yy.cpp banglishpp_flex_main.cpp \
    -o banglishpp
```

---

## ▶️ Run the Compiler

Compile a Banglish++ program:

```bash
./banglishpp nahole_jodi_demo.bpp generated.py
```

A successful compilation produces:

```text
Compilation successful.
Generated Python file: generated.py
```

Run the generated Python program:

```bash
python3 generated.py
```

---

## 🧪 Testing

The repository includes a regression test suite covering both valid and invalid programs.

Run:

```bash
make test
```

The test suite checks cases involving:

* Arithmetic precedence
* Data types
* Assignments
* `jodi / nahole`
* `jokhon`
* `cholo / thamvai`
* Invalid expressions
* Type errors
* Missing semicolons
* Missing braces
* Multiple syntax errors
* Python keyword identifiers
* Leading-zero numbers
* Division behavior
* Empty source files

Successful generated Python files are additionally validated using:

```bash
python3 -m py_compile
```

---

## 📝 Example Banglish++ Program

```text
shonkha marks = 75;
doshomik average = marks / 2.0;

jodi (marks >= 90) {
    lehkho("Excellent");
}
nahole jodi (marks >= 70 && marks < 90) {
    lehkho("Good");
}
nahole {
    lehkho("Needs Improvement");
}

jokhon (marks < 80) {
    marks = marks + 1;
}
```

The compiler processes the program through all compiler phases and generates equivalent Python 3 source code.

---

## 🧩 Compiler Components

### 1. Lexer

**File:**

```text
banglishpp_lexer.l
```

Converts source characters into tokens.

### 2. Parser

**File:**

```text
part04_parser.cpp
```

Builds the AST using recursive-descent parsing.

### 3. AST

**File:**

```text
part03_ast_types.cpp
```

Defines the structures used to represent the program.

### 4. Type Checker

**File:**

```text
part05_type_checker.cpp
```

Performs semantic validation and type checking.

### 5. Python Generator

**File:**

```text
part06_python_code_generator.cpp
```

Converts the validated AST into Python 3 source code.

### 6. Compiler Driver

**File:**

```text
banglishpp_flex_main.cpp
```

Connects the Flex lexer, parser, type checker, and code generator.

---

## 🛡️ Error Handling

Banglish++ is designed to reject invalid programs safely.

The compiler handles:

```text
Lexical Errors
      ↓
Syntax Errors
      ↓
Semantic / Type Errors
      ↓
Compilation Failure
```

Instead of crashing on malformed input, the compiler reports errors with source location information whenever possible.

---

## 📚 Educational Purpose

This project demonstrates the major stages of a compiler:

```text
Source Code
    ↓
Lexical Analysis
    ↓
Syntax Analysis
    ↓
Abstract Syntax Tree
    ↓
Semantic Analysis
    ↓
Code Generation
    ↓
Python 3
```

It was designed as a practical implementation of concepts from **Compiler Design and Construction**.

---

## 📌 Project Status

### Implemented

* [x] Flex/Lex lexer
* [x] Recursive Descent parser
* [x] Abstract Syntax Tree
* [x] Integer, floating-point, and string types
* [x] Type checking
* [x] Arithmetic operators
* [x] Operator precedence
* [x] Assignment
* [x] Conditional statements
* [x] While loops
* [x] Break and continue
* [x] Basic syntax error recovery
* [x] Python code generation
* [x] Python-safe identifiers
* [x] Numeric literal normalization
* [x] Regression test suite

---

## 👨‍💻 Authors

**Mahmudul Hasan Sakib**
Student ID: `0182320012101417`

**Jasmin Bagom**
Student ID: `0182320012101409`

**Sakil Ahmed**
Student ID: `0182320012101403`

---

## 📄 License

This project was developed for **educational and academic purposes** as part of a Compiler Design and Construction course.

---

⭐ **Banglish++ — Making Compiler Design More Bangladeshi**
