#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  template <typename FormatR, typename FormatX, typename FormatY, typename ProjSpec>
  FormatR MaximumMagnitude(FormatX x, FormatY y, ProjSpec projSpec)
  {
    if (x.isnan())
      return FormatR::nan;
    if (y.isnan())
      return FormatR::nan;
    // Any inf: the inf with largest magnitude (posinf first)
    if (x.isposinf() || y.isposinf())
      return FormatR::inf;
    if (x.isneginf() || y.isneginf())
      return FormatR::ninf;
    mpfr_float dx = Decode<FormatX>(x);
    mpfr_float dy = Decode<FormatY>(y);
    mpfr_float ax = abs(dx);
    mpfr_float ay = abs(dy);
    if (ax > ay)
      return Project<FormatR>(dx, projSpec);
    if (ax < ay)
      return Project<FormatR>(dy, projSpec);
    // Equal magnitude: return max(x, y)
    return Project<FormatR>(dx < dy ? dy : dx, projSpec);
  }
} // namespace p3109
