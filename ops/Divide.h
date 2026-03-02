#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  template <typename FormatR, typename FormatX, typename FormatY, typename ProjSpec>
  FormatR Divide(FormatX x, FormatY y, ProjSpec projSpec)
  {
    if (x.isnan())
      return FormatR::nan;
    if (y.isnan())
      return FormatR::nan;
    // inf / inf = NaN
    if (x.isinf() && y.isinf())
      return FormatR::nan;

    mpfr_float dx = Decode<FormatX>(x);
    mpfr_float dy = Decode<FormatY>(y);

    // X / 0 = NaN
    if (dy == 0)
      return FormatR::nan;
    // inf / Y: sign determines result
    if (x.isinf())
    {
      bool same_sign = (dx > 0) == (dy > 0);
      return same_sign ? FormatR::inf : FormatR::ninf;
    }
    // X / inf = 0
    if (y.isinf())
      return Project<FormatR>(mpfr_float(0), projSpec);

    return Project<FormatR>(dx / dy, projSpec);
  }
} // namespace p3109
