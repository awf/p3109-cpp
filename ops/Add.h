#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  // Add<FormatR>(x, y, projSpec) -> r
  //
  // Structured definition (main.tex §Add):
  // - Parameters:
  //     FormatR   = format of result r
  //     FormatX   = format of operand x (deduced from x)
  //     FormatY   = format of operand y (deduced from y)
  //     ProjSpec  = projection specification type (ProjectionSpec<...>)
  // - Operands:
  //     x         = P3109 value, format FormatX
  //     y         = P3109 value, format FormatY
  //     projSpec  = ProjSpec (projection specification)
  // - Result:
  //     r         = P3109 value, format FormatR
  //
  template <typename FormatR, typename FormatX, typename FormatY, typename ProjSpec>
  FormatR Add(FormatX x, FormatY y, ProjSpec projSpec)
  {
    // Canonical Add cases (from wAdd.tex):
    // 1. Add(NaN, any) -> NaN
    if (x.isnan())
      return FormatR::nan;
    // 2. Add(any, NaN) -> NaN
    if (y.isnan())
      return FormatR::nan;
    // 3. Add(+inf, -inf) -> NaN
    if (x.isposinf() && y.isneginf())
      return FormatR::nan;
    // 4. Add(-inf, +inf) -> NaN
    if (x.isneginf() && y.isposinf())
      return FormatR::nan;
    // 5. Add(+inf, any) -> +inf
    if (x.isposinf())
      return FormatR::inf;
    // 6. Add(any, +inf) -> +inf
    if (y.isposinf())
      return FormatR::inf;
    // 7. Add(-inf, any) -> -inf
    if (x.isneginf())
      return FormatR::ninf;
    // 8. Add(any, -inf) -> -inf
    if (y.isneginf())
      return FormatR::ninf;
    // 9. Add(X, Y) -> X + Y
    mpfr_float dx = Decode<FormatX>(x);
    mpfr_float dy = Decode<FormatY>(y);
    return Project<FormatR>(dx + dy, projSpec);
  }
} // namespace p3109
