#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  template <typename FormatR, typename FormatX, typename ProjSpec>
  FormatR TanPi(FormatX x, ProjSpec projSpec)
  {
    if (x.isnan())
      return FormatR::nan;
    if (x.isinf())
      return FormatR::nan;
    mpfr_float decoded = Decode<FormatX>(x);
    mpfr_float c = cos(decoded * mpfr_pi());
    if (c == 0)
      return FormatR::nan;
    return Project<FormatR>(tan(decoded * mpfr_pi()), projSpec);
  }
} // namespace p3109
