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
