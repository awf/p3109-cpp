#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  // Clamp(x, lo, hi) — clamp x to [lo, hi]
  template <typename FormatR, typename FormatX, typename FormatLo, typename FormatHi, typename ProjSpec>
  FormatR Clamp(FormatX x, FormatLo lo, FormatHi hi, ProjSpec projSpec)
  {
    if (x.isnan())
      return FormatR::nan;
    if (lo.isnan())
      return FormatR::nan;
    if (hi.isnan())
      return FormatR::nan;

    mpfr_float dlo = lo.isinf() ? (lo.isposinf() ? mpfr_inf() : -mpfr_inf()) : Decode<FormatLo>(lo);
    mpfr_float dhi = hi.isinf() ? (hi.isposinf() ? mpfr_inf() : -mpfr_inf()) : Decode<FormatHi>(hi);

    // lo > hi → NaN (includes +inf,-inf and +inf,finite etc.)
    if (dlo > dhi)
      return FormatR::nan;

    // lo == hi == ±inf
    if (lo.isposinf() && hi.isposinf())
      return FormatR::inf;
    if (lo.isneginf() && hi.isneginf())
      return FormatR::ninf;

    // x = +inf
    if (x.isposinf())
    {
      if (hi.isposinf())
        return FormatR::inf;
      return Project<FormatR>(dhi, projSpec);
    }
    // x = -inf
    if (x.isneginf())
    {
      if (lo.isneginf())
        return FormatR::ninf;
      return Project<FormatR>(dlo, projSpec);
    }

    // x finite
    mpfr_float dx = Decode<FormatX>(x);

    if (!lo.isneginf() && dx <= dlo)
      return Project<FormatR>(dlo, projSpec);
    if (!hi.isposinf() && dx >= dhi)
      return Project<FormatR>(dhi, projSpec);
    return Project<FormatR>(dx, projSpec);
  }
} // namespace p3109
