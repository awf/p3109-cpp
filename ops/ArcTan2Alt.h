#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  // ArcTan2Alt(y, x) — IEEE 754-compatible atan2
  template <typename FormatR, typename FormatY, typename FormatX, typename ProjSpec>
  FormatR ArcTan2Alt(FormatY y, FormatX x, ProjSpec projSpec)
  {
    if (y.isnan())
      return FormatR::nan;
    if (x.isnan())
      return FormatR::nan;

    const mpfr_float pi = mpfr_pi();
    mpfr_float dy = y.isinf() ? mpfr_float(0) : Decode<FormatY>(y);
    mpfr_float dx = x.isinf() ? mpfr_float(0) : Decode<FormatX>(x);

    // Both inf: specific quadrant results
    if (y.isinf() && x.isinf())
    {
      bool yp = y.isposinf();
      bool xp = x.isposinf();
      if (yp && xp)
        return Project<FormatR>(pi / 4, projSpec);
      if (yp && !xp)
        return Project<FormatR>(mpfr_float(3) * pi / 4, projSpec);
      if (!yp && xp)
        return Project<FormatR>(-pi / 4, projSpec);
      return Project<FormatR>(mpfr_float(-3) * pi / 4, projSpec);
    }

    // y=0 cases
    if (!y.isinf() && dy == 0)
    {
      if (x.isneginf() || (!x.isinf() && dx < 0))
        return Project<FormatR>(pi, projSpec);
      // x >= 0 or +inf: result is 0
      return Project<FormatR>(mpfr_float(0), projSpec);
    }

    // y finite or inf, x = ±inf
    if (x.isneginf())
      return dy > 0 || y.isposinf() ? Project<FormatR>(pi, projSpec) : Project<FormatR>(-pi, projSpec);
    if (x.isposinf())
      return Project<FormatR>(mpfr_float(0), projSpec);

    // y = ±inf, x finite
    if (y.isposinf())
      return Project<FormatR>(pi / 2, projSpec);
    if (y.isneginf())
      return Project<FormatR>(-pi / 2, projSpec);

    // Both finite
    if (dx == 0 && dy == 0)
      return Project<FormatR>(mpfr_float(0), projSpec);
    if (dx == 0)
      return dy > 0 ? Project<FormatR>(pi / 2, projSpec) : Project<FormatR>(-pi / 2, projSpec);
    if (dx > 0)
      return Project<FormatR>(atan(dy / dx), projSpec);
    if (dy > 0)
      return Project<FormatR>(atan(dy / dx) + pi, projSpec);
    return Project<FormatR>(atan(dy / dx) - pi, projSpec);
  }
} // namespace p3109
