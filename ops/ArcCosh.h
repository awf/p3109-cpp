#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  template <typename FormatR, typename FormatX, typename ProjSpec>
  FormatR ArcCosh(FormatX x, ProjSpec projSpec)
  {
    if (x.isnan())
      return FormatR::nan;
    if (x.isposinf())
      return FormatR::inf;
    if (x.isneginf())
      return FormatR::nan;
    mpfr_float decoded = Decode<FormatX>(x);
    if (decoded < 1)
      return FormatR::nan;
    return Project<FormatR>(acosh(decoded), projSpec);
  }
} // namespace p3109
