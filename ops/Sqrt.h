#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  // Sqrt<FormatR>(x, projSpec) -> r
  //
  // Structured definition (main.tex §Sqrt):
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
  template <typename FormatR, typename FormatX,
    typename ProjSpec = ProjectionSpec<NearestTiesToEven, SaturationMode::SatFinite>>
  FormatR Sqrt(FormatX x, ProjSpec projSpec = ProjSpec{})
  {
    // Canonical Sqrt cases (from wSqrt.tex):
    // 1. Sqrt(NaN) -> NaN
    if (x.isnan())
      return FormatR::nan;
    // 2. Sqrt(-inf) -> NaN
    if (x.isneginf())
      return FormatR::nan;
    // 3. Sqrt(X), X < 0 -> NaN
    mpfr_float decoded = Decode<FormatX>(x);
    if (decoded < 0)
      return FormatR::nan;
    // 4. Sqrt(+inf) -> +inf
    if (x.isposinf())
      return FormatR::inf;
    // 5. Sqrt(X) -> sqrt(X)
    mpfr_float sqrted = sqrt(decoded);
    return Project<FormatR>(sqrted, projSpec);
  }
} // namespace p3109
