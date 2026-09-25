#!/usr/bin/env bash
set -u
COMPILER=${1:-./banglishpp_standalone}
PASS=0
FAIL=0

tmpdir=$(mktemp -d)
trap 'rm -rf "$tmpdir"' EXIT

run_success() {
    local src="$1"
    local out="$tmpdir/out.py"
    if "$COMPILER" "$src" "$out" >"$tmpdir/stdout" 2>"$tmpdir/stderr" && python3 -m py_compile "$out" >/dev/null 2>&1; then
        echo "PASS  $src"
        PASS=$((PASS + 1))
    else
        echo "FAIL  $src"
        cat "$tmpdir/stderr"
        FAIL=$((FAIL + 1))
    fi
}

run_failure() {
    local src="$1"
    if "$COMPILER" "$src" "$tmpdir/out.py" >"$tmpdir/stdout" 2>"$tmpdir/stderr"; then
        echo "FAIL  $src (expected compiler error)"
        FAIL=$((FAIL + 1))
    else
        echo "PASS  $src (rejected safely)"
        PASS=$((PASS + 1))
    fi
}

run_success tests/valid.bpp
run_success tests/valid_types.bpp
run_success tests/python_keyword.bpp
run_success tests/leading_zero.bpp
run_success tests/division_ok.bpp
run_success tests/empty.bpp
run_failure tests/type_errors.bpp
run_failure tests/division_type_error.bpp
run_failure tests/bad_expr.bpp
run_failure tests/missing_semicolon.bpp
run_failure tests/missing_brace.bpp
run_failure tests/multi_errors.bpp
run_failure invalid_loop_control.bpp

run_runtime() {
    local src="$1"
    local out="$tmpdir/runtime.py"
    local expected="$tmpdir/expected.txt"
    local actual="$tmpdir/actual.txt"

    cat >"$expected" <<'EOF'
Good
1
3
5
6
7
8
EOF

    if "$COMPILER" "$src" "$out" --no-run >"$tmpdir/runtime-compiler.stdout" 2>"$tmpdir/runtime-compiler.stderr" \
        && python3 "$out" >"$actual" 2>"$tmpdir/runtime-python.stderr" \
        && diff -u "$expected" "$actual" >/dev/null; then
        echo "PASS  $src (runtime output)"
        PASS=$((PASS + 1))
    else
        echo "FAIL  $src (runtime output)"
        cat "$tmpdir/runtime-compiler.stderr"
        cat "$tmpdir/runtime-python.stderr"
        cat "$actual" 2>/dev/null || true
        FAIL=$((FAIL + 1))
    fi
}

run_runtime nahole_jodi_demo.bpp

echo
printf 'Passed: %d\nFailed: %d\n' "$PASS" "$FAIL"
[ "$FAIL" -eq 0 ]
