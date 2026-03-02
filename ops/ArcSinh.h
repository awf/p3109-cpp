#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  template <typename FormatR, typename FormatX, typename ProjSpec>
  FormatR ArcSinh(FormatX x, ProjSpec projSpec)
  {
    if (x.isnan())
      return FormatR::nan;
    if (x.isposinf())
      return FormatR::inf;
    if (x.isneginf())
      return FormatR::ninf;
    mpfr_float decoded = Decode<FormatX>(x);
    return Project<FormatR>(asinh(decoded), projSpec);
  }
} // namespace p3109
