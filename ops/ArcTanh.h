#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  template <typename FormatR, typename FormatX, typename ProjSpec>
  FormatR ArcTanh(FormatX x, ProjSpec projSpec)
  {
    if (x.isnan())
      return FormatR::nan;
    if (x.isinf())
      return FormatR::nan;
    mpfr_float decoded = Decode<FormatX>(x);
    if (decoded == 1)
      return FormatR::inf;
    if (decoded == -1)
      return FormatR::ninf;
    if (decoded < -1 || decoded > 1)
      return FormatR::nan;
    return Project<FormatR>(atanh(decoded), projSpec);
  }
} // namespace p3109
