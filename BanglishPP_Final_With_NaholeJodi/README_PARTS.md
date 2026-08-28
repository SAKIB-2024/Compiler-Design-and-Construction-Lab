# Banglish++ updated compiler

Added:
- `&&` logical AND
- `||` logical OR
- `nahole jodi` else-if
- `cholo;` continue
- `thamvai;` break

## Structure

part01_token.cpp
part02_lexer.cpp
part03_ast_types.cpp
part04_parser.cpp
part05_type_checker.cpp
part06_python_code_generator.cpp
part07_utils.cpp
part08_main.cpp

`banglishpp_all_parts.cpp` is the single complete buildable compiler.

## Mac build

```bash
g++ -std=c++17 -O2 -Wall -Wextra -pedantic banglishpp_all_parts.cpp -o banglishpp
```

or on macOS:

```bash
clang++ -std=c++17 -O2 -Wall -Wextra -pedantic banglishpp_all_parts.cpp -o banglishpp
```

## Run

```bash
./banglishpp nahole_jodi_demo.bpp generated.py
python3 generated.py
```

## `nahole jodi`

```text
jodi (a > 90) {
    lehkho("Excellent");
} nahole jodi (a > 80) {
    lehkho("Very Good");
} nahole {
    lehkho("Good or below");
}
```

The generated Python uses `if / elif / else`.

`cholo` and `thamvai` remain valid only inside `jokhon` loops.
