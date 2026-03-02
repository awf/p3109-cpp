#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  // ArcTan2Pi(y, x) — atan2 scaled by 1/π
  template <typename FormatR, typename FormatY, typename FormatX, typename ProjSpec>
  FormatR ArcTan2Pi(FormatY y, FormatX x, ProjSpec projSpec)
  {
    if (y.isnan())
      return FormatR::nan;
    if (x.isnan())
      return FormatR::nan;

    const mpfr_float pi = mpfr_pi();

    // inf,inf → NaN
    if (y.isinf() && x.isinf())
      return FormatR::nan;

    mpfr_float dy = y.isinf() ? mpfr_float(0) : Decode<FormatY>(y);
    mpfr_float dx = x.isinf() ? mpfr_float(0) : Decode<FormatX>(x);

    // 0,0 → NaN
    if (!y.isinf() && !x.isinf() && dy == 0 && dx == 0)
      return FormatR::nan;

    // (any, +inf) → 0
    if (x.isposinf())
      return Project<FormatR>(mpfr_float(0), projSpec);
    // (0, X) X > 0 → 0
    if (!y.isinf() && dy == 0 && dx > 0)
      return Project<FormatR>(mpfr_float(0), projSpec);
    // (+inf, any) → 1/2
    if (y.isposinf())
      return Project<FormatR>(mpfr_float("0.5"), projSpec);
    // (Y, 0) Y > 0 → 1/2
    if (!x.isinf() && dx == 0 && dy > 0)
      return Project<FormatR>(mpfr_float("0.5"), projSpec);
    // (any, -inf) → 1
    if (x.isneginf())
      return Project<FormatR>(mpfr_float(1), projSpec);
    // (0, X) X < 0 → 1
    if (!y.isinf() && dy == 0 && dx < 0)
      return Project<FormatR>(mpfr_float(1), projSpec);
    // (-inf, any) → -1/2
    if (y.isneginf())
      return Project<FormatR>(mpfr_float("-0.5"), projSpec);
    // (Y, 0) Y < 0 → -1/2
    if (!x.isinf() && dx == 0 && dy < 0)
      return Project<FormatR>(mpfr_float("-0.5"), projSpec);

    // Finite cases
    if (dx > 0)
      return Project<FormatR>(atan(dy / dx) / pi, projSpec);
    if (dy > 0 && dx < 0)
      return Project<FormatR>((atan(dy / dx) + pi) / pi, projSpec);
    return Project<FormatR>((atan(dy / dx) - pi) / pi, projSpec);
  }
} // namespace p3109
