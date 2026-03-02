#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  // Abs<FormatR>(x, projSpec) -> r
  //
  // Structured definition (main.tex §Abs):
  // - Parameters:
  //     FormatR   = format of result r
  //     FormatX   = format of operand x (deduced from x)
  //     ProjSpec  = projection specification type (ProjectionSpec<...>)
  // - Operands:
  //     x         = P3109 value, format FormatX
  //     projSpec  = ProjSpec (projection specification)
  // - Result:
  //     r         = P3109 value, format FormatR
  //
  template <typename FormatR, typename FormatX, typename ProjSpec>
  FormatR Abs(FormatX x, ProjSpec projSpec)
  {
    // Canonical Abs cases (from wAbs.tex):
    // 1. Abs(NaN) -> NaN
    if (x.isnan())
      return FormatR::nan;
    // 2. Abs(-inf) -> +inf
    if (x.isneginf())
      return FormatR::inf;
    // 3. Abs(+inf) -> +inf
    if (x.isposinf())
      return FormatR::inf;
    // 4. Abs(X) -> |X|
    mpfr_float decoded = Decode<FormatX>(x);
    return Project<FormatR>(abs(decoded), projSpec);
  }
} // namespace p3109
