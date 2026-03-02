#pragma once

#include "ops/Decode.h"
#include "p3109.h"

namespace p3109 {
  // CompareEqual(x, y) -> bool
  //
  // Structured definition (main.tex §CompareEqual):
  // - Parameters:
  //     FormatX   = format of operand x (deduced from x)
  //     FormatY   = format of operand y (deduced from y)
  // - Operands:
  //     x         = P3109 value, format FormatX
  //     y         = P3109 value, format FormatY
  // - Result:
  //     bool
  //
  // Note: Comparisons do not take a ProjectionSpec (no rounding/projection).
  //
  template <typename FormatX, typename FormatY>
  bool CompareEqual(FormatX x, FormatY y)
  {
    // Canonical CompareEqual cases (from wCompareEqual.tex):
    // 1. CompareEqual(NaN, any) -> false
    if (x.isnan())
      return false;
    // 2. CompareEqual(any, NaN) -> false
    if (y.isnan())
      return false;
    // 3. CompareEqual(+inf, +inf) -> true
    if (x.isposinf() && y.isposinf())
      return true;
    // 4. CompareEqual(-inf, -inf) -> true
    if (x.isneginf() && y.isneginf())
      return true;
    // 5. CompareEqual(+inf, any) -> false
    if (x.isposinf())
      return false;
    // 6. CompareEqual(-inf, any) -> false
    if (x.isneginf())
      return false;
    // 7. CompareEqual(any, -inf) -> false
    if (y.isneginf())
      return false;
    // 8. CompareEqual(any, +inf) -> false
    if (y.isposinf())
      return false;
    // 9. CompareEqual(X, Y) -> X == Y
    mpfr_float dx = Decode<FormatX>(x);
    mpfr_float dy = Decode<FormatY>(y);
    return dx == dy;
  }
} // namespace p3109
