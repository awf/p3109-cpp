#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  // ArcTan2(y, x) — note: y is first argument, x is second (standard atan2 convention)
  template <typename FormatR, typename FormatY, typename FormatX, typename ProjSpec>
  FormatR ArcTan2(FormatY y, FormatX x, ProjSpec projSpec)
  {
    if (y.isnan())
      return FormatR::nan;
    if (x.isnan())
      return FormatR::nan;

    const mpfr_float pi = mpfr_pi();

    // inf,inf cases → NaN
    if (y.isinf() && x.isinf())
      return FormatR::nan;
    // 0,0 → NaN
    mpfr_float dy = y.isinf() ? mpfr_float(0) : Decode<FormatY>(y);
    mpfr_float dx = x.isinf() ? mpfr_float(0) : Decode<FormatX>(x);

    if (!y.isinf() && !x.isinf() && dy == 0 && dx == 0)
      return FormatR::nan;

    // (any, +inf) → 0
    if (x.isposinf())
      return Project<FormatR>(mpfr_float(0), projSpec);
    // (0, X) X > 0 → 0
    if (!y.isinf() && dy == 0 && dx > 0)
      return Project<FormatR>(mpfr_float(0), projSpec);
    // (+inf, any) → π/2
    if (y.isposinf())
      return Project<FormatR>(pi / 2, projSpec);
    // (Y, 0) Y > 0 → π/2
    if (!x.isinf() && dx == 0 && dy > 0)
      return Project<FormatR>(pi / 2, projSpec);
    // (any, -inf) → π
    if (x.isneginf())
      return Project<FormatR>(pi, projSpec);
    // (0, X) X < 0 → π
    if (!y.isinf() && dy == 0 && dx < 0)
      return Project<FormatR>(pi, projSpec);
    // (-inf, any) → -π/2
    if (y.isneginf())
      return Project<FormatR>(-pi / 2, projSpec);
    // (Y, 0) Y < 0 → -π/2
    if (!x.isinf() && dx == 0 && dy < 0)
      return Project<FormatR>(-pi / 2, projSpec);

    // Finite cases
    if (dx > 0)
      return Project<FormatR>(atan(dy / dx), projSpec);
    if (dy > 0 && dx < 0)
      return Project<FormatR>(atan(dy / dx) + pi, projSpec);
    // dy < 0 && dx < 0
    return Project<FormatR>(atan(dy / dx) - pi, projSpec);
  }
} // namespace p3109
