# p3109-cpp

[![C++ CI](https://github.com/awf/p3109-cpp/actions/workflows/ci.yml/badge.svg)](https://github.com/awf/p3109-cpp/actions/workflows/ci.yml)

Reference C++ implementation and tests for `p3109` decoding.

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Test

```bash
ctest --test-dir build --output-on-failure
```

## CI

The CI workflow runs on push/PR with a matrix that includes:
- Linux Release build + tests
- Linux Sanitizers build (`ASan` + `UBSan`) + tests
- macOS Release build + tests

## Local pre-commit sanitizers

This repo includes a tracked pre-commit hook at [.githooks/pre-commit](.githooks/pre-commit) that runs sanitizer build/tests via [scripts/run-sanitizers.sh](scripts/run-sanitizers.sh).

Enable it once per clone:

```bash
git config core.hooksPath .githooks
```

Run sanitizer checks manually anytime:

```bash
./scripts/run-sanitizers.sh
```
