#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  template <typename FormatR, typename FormatX, typename ProjSpec>
  FormatR Tan(FormatX x, ProjSpec projSpec)
  {
    if (x.isnan())
      return FormatR::nan;
    if (x.isinf())
      return FormatR::nan;
    mpfr_float decoded = Decode<FormatX>(x);
    mpfr_float c = cos(decoded);
    if (c == 0)
      return FormatR::nan;
    return Project<FormatR>(tan(decoded), projSpec);
  }
} // namespace p3109
