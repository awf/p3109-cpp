#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  template <typename FormatR, typename FormatX, typename FormatY, typename ProjSpec>
  FormatR MinimumMagnitude(FormatX x, FormatY y, ProjSpec projSpec)
  {
    if (x.isnan())
      return FormatR::nan;
    if (y.isnan())
      return FormatR::nan;
    // Both inf
    if (x.isinf() && y.isinf())
    {
      if (x.isneginf() || y.isneginf())
        return FormatR::ninf;
      return FormatR::inf;
    }
    // One inf: return the other (finite or -inf)
    if (x.isinf())
    {
      if (y.isneginf())
        return FormatR::ninf;
      if (y.isposinf())
        return FormatR::inf;
      return Project<FormatR>(Decode<FormatY>(y), projSpec);
    }
    if (y.isinf())
    {
      if (x.isneginf())
        return FormatR::ninf;
      if (x.isposinf())
        return FormatR::inf;
      return Project<FormatR>(Decode<FormatX>(x), projSpec);
    }
    mpfr_float dx = Decode<FormatX>(x);
    mpfr_float dy = Decode<FormatY>(y);
    mpfr_float ax = abs(dx);
    mpfr_float ay = abs(dy);
    if (ax < ay)
      return Project<FormatR>(dx, projSpec);
    if (ax > ay)
      return Project<FormatR>(dy, projSpec);
    // Equal magnitude: return min(x, y)
    return Project<FormatR>(dx < dy ? dx : dy, projSpec);
  }
} // namespace p3109
