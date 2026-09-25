# Banglish++ Compiler

Banglish++ is a small C++17 compiler that translates Banglish++ source code (`.bpp`) to Python 3.

## Rubric coverage

- **Lexer:** Lex/Flex — `banglishpp_lexer.l`
- **Parser:** Recursive Descent — `part04_parser.cpp`
- **Implementation:** C++17
- **Target:** Python 3
- **Data types:** `shonkha`, `doshomik`, `shobdo`
- **Arithmetic:** `+ - * / %` with precedence
- **Assignment:** `=`
- **Conditional:** `jodi / nahole jodi / nahole`
- **Loop:** `jokhon` (while)
- **Loop control:** `cholo` / `thamvai`
- **Error recovery:** semicolon / next line / block-boundary synchronization
- **Type checking:** declarations, assignments, arithmetic, comparisons, logical operators, conditions
- **Target hardening:** Python-safe identifiers and numeric literals

## Build using the required Flex lexer

Prerequisites:

- Flex
- g++ or another C++17 compiler
- Python 3 (for target validation/tests)

Then run:

```bash
make
```

This performs:

```bash
flex -o lex.yy.cpp banglishpp_lexer.l
g++ -std=c++17 -O2 -Wall -Wextra -pedantic lex.yy.cpp banglishpp_flex_main.cpp -o banglishpp
```

Run the compiler (it generates Python and runs it automatically):

```bash
./banglishpp nahole_jodi_demo.bpp generated.py
```

For compile-only mode, use `--no-run`:

```bash
./banglishpp nahole_jodi_demo.bpp generated.py --no-run
```

## Standalone fallback build

The project also contains the all-in-one source file for environments where Flex is unavailable:

```bash
make standalone
./banglishpp_standalone nahole_jodi_demo.bpp generated.py
```

For the course submission, use the **Flex build** because the lexer requirement explicitly specifies Lex/Flex.

## Tests

```bash
make test
```

The tests compile valid programs, reject invalid programs safely, syntax-check successful Python output, and verify the demo runtime output.

## Language example

```text
shonkha a = 75;

doshomik b = a / 2.0;

jodi (a > 90) {
    lehkho("Excellent");
} nahole jodi (a > 70 && a < 80) {
    lehkho("Good");
} nahole {
    lehkho("Needs Improvement");
}

jokhon (a < 80) {
    a = a + 1;
}
```

## File structure

```text
part01_token.cpp              Original token definitions
part01_token.hpp              Shared token header for Flex build
part02_lexer.cpp              Standalone/fallback handwritten lexer
part03_ast_types.cpp          AST and type definitions
part04_parser.cpp             Recursive Descent parser
part05_type_checker.cpp       Semantic/type checking
part06_python_code_generator.cpp  Python target generator
part07_utils.cpp              File utilities
part08_main.cpp               Standalone compiler main
banglishpp_all_parts.cpp      Standalone all-in-one compiler
banglishpp_lexer.l             Flex/Lex lexer specification
banglishpp_flex_main.cpp       Flex-based compiler main
Makefile                      Reproducible build/test commands
PROJECT_REPORT.md             Project report
```
