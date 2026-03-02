#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  template <typename FormatR, typename FormatX, typename ProjSpec>
  FormatR RSqrt(FormatX x, ProjSpec projSpec)
  {
    if (x.isnan())
      return FormatR::nan;
    if (x.isneginf())
      return FormatR::nan;
    mpfr_float decoded = Decode<FormatX>(x);
    if (decoded <= 0)
      return FormatR::nan;
    if (x.isposinf())
      return Project<FormatR>(mpfr_float(0), projSpec);
    return Project<FormatR>(mpfr_float(1) / sqrt(decoded), projSpec);
  }
} // namespace p3109
