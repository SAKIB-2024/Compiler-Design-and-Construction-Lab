CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic

TARGET = banglishpp
FLEX_SRC = banglishpp_lexer.l
FLEX_GEN = lex.yy.cpp

.PHONY: all flex clean test standalone

all: $(TARGET)

$(FLEX_GEN): $(FLEX_SRC) part01_token.hpp
	flex -o $@ $<

$(TARGET): $(FLEX_GEN) banglishpp_flex_main.cpp part01_token.hpp part03_ast_types.cpp part04_parser.cpp part05_type_checker.cpp part06_python_code_generator.cpp part07_utils.cpp
	$(CXX) $(CXXFLAGS) $(FLEX_GEN) banglishpp_flex_main.cpp -o $@

flex: $(TARGET)
	@echo "Flex lexer build complete: ./$(TARGET)"

standalone: banglishpp_all_parts.cpp
	$(CXX) $(CXXFLAGS) banglishpp_all_parts.cpp -o banglishpp_standalone

test: standalone
	bash tests/run_tests.sh ./banglishpp_standalone

clean:
	rm -f $(TARGET) $(FLEX_GEN) banglishpp_standalone generated.py tests/*.pyc
