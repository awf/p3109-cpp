#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  template <typename FormatR, typename FormatX, typename ProjSpec>
  FormatR Negate(FormatX x, ProjSpec projSpec)
  {
    if (x.isnan())
      return FormatR::nan;
    if (x.isneginf())
      return FormatR::inf;
    if (x.isposinf())
      return FormatR::ninf;
    mpfr_float decoded = Decode<FormatX>(x);
    return Project<FormatR>(-decoded, projSpec);
  }
} // namespace p3109
