#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  template <typename FormatR, typename FormatX, typename ProjSpec>
  FormatR Exp(FormatX x, ProjSpec projSpec)
  {
    if (x.isnan())
      return FormatR::nan;
    if (x.isposinf())
      return FormatR::inf;
    if (x.isneginf())
      return Project<FormatR>(mpfr_float(0), projSpec);
    mpfr_float decoded = Decode<FormatX>(x);
    return Project<FormatR>(exp(decoded), projSpec);
  }
} // namespace p3109
