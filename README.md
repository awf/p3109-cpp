# p3109-cpp

[![C++ CI](https://github.com/awf/p3109-cpp/actions/workflows/ci.yml/badge.svg)](https://github.com/awf/p3109-cpp/actions/workflows/ci.yml)

Reference C++ implementation of the interim report of the IEEE Standards Association working group P3109.

P3109 defines a family of floating point formats and operations parameterized over size, precision, signedness and the presence or absence of infinities.

This package defines a datatype
```c++
  binary<uint K, uint P, Signedness sigma, Domain delta>
```
with
```c++
  enum Signedness { Signed, Unsigned };
  enum Domain { Finite, Extended };
```

And operations of the form
```c++
Add<FormatResult, FormatX, FormatY, ProjectionSpec>(FormatX x, FormatY y) -> FormatResult
```

where `ProjectionSpec` specifies the rounding and saturation modes to apply in the operation:
```c++
  struct ProjectionSpec {
    RoundingMode round;
    SaturationMode saturate;
  };
```
which are
```c++
  enum SaturationMode {
    SatFinite,
    SatPropagate,
    OvfInf,
  };
```
and a class-like enum
```
  RoundingMode
  - TowardZero
  - TowardMaxMagnitude
  - TowardPositive
  - TowardNegative
  - NearestTiesToAway
  - NearestTiesToEven
  - ToOdd
  - StochasticA<N>
  - StochasticB<N>
  - StochasticC<N>
```
where the `Stochastic` variants carry a payload of random bits.

Hence a call to `Sqrt` with rounding toward zero and saturation to finite values might look like
```c++
   p3109::Sqrt<FormatResult>(x,    ProjectionSpec<TowardZero,SatFinite>>{})
```

And using stochastic rounding:
```c++
   p3109::Sqrt<FormatResult>(x,    ProjectionSpec<StochasticB<4>,SatFinite>>{{7}})
```

## Template parameter order
Note that the result format parameter appears first in the parameter list, while in the P3109 interim report, it is listed last.
This is to enable template parameter type deduction on the arguments.

# For developers

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

This repo includes a tracked pre-commit hook at [.githooks/pre-commit](.githooks/pre-commit) that:
- runs `clang-format` on staged C/C++ files (and re-stages them)
- runs sanitizer build/tests via [scripts/run-sanitizers.sh](scripts/run-sanitizers.sh)

Enable it once per clone:

```bash
git config core.hooksPath .githooks
```

Run sanitizer checks manually anytime:

```bash
./scripts/run-sanitizers.sh
```
