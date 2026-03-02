#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  template <typename FormatR, typename FormatX, typename FormatY, typename ProjSpec>
  FormatR CopySign(FormatX x, FormatY y, ProjSpec projSpec)
  {
    // CopySign(NaN, y) -> NaN
    if (x.isnan())
      return FormatR::nan;
    // CopySign(x, NaN) -> NaN
    if (y.isnan())
      return FormatR::nan;

    mpfr_float dy = y.isinf() ? (y.isposinf() ? mpfr_float(1) : mpfr_float(-1)) : Decode<FormatY>(y);
    bool y_nonneg = (dy >= 0);

    // CopySign(±inf, y≥0) -> +inf; CopySign(±inf, y<0) -> -inf
    if (x.isinf())
      return y_nonneg ? FormatR::inf : FormatR::ninf;

    // CopySign(x, y≥0) -> |x|; CopySign(x, y<0) -> -|x|
    mpfr_float dx = Decode<FormatX>(x);
    mpfr_float absdx = abs(dx);
    mpfr_float result = y_nonneg ? absdx : -absdx;
    return Project<FormatR>(result, projSpec);
  }
} // namespace p3109
