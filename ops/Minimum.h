#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  template <typename FormatR, typename FormatX, typename FormatY, typename ProjSpec>
  FormatR Minimum(FormatX x, FormatY y, ProjSpec projSpec)
  {
    if (x.isnan())
      return FormatR::nan;
    if (y.isnan())
      return FormatR::nan;
    if (x.isneginf() || y.isneginf())
      return FormatR::ninf;
    if (x.isposinf())
    {
      if (y.isposinf())
        return FormatR::inf;
      return Project<FormatR>(Decode<FormatY>(y), projSpec);
    }
    if (y.isposinf())
      return Project<FormatR>(Decode<FormatX>(x), projSpec);
    mpfr_float dx = Decode<FormatX>(x);
    mpfr_float dy = Decode<FormatY>(y);
    return Project<FormatR>(dx < dy ? dx : dy, projSpec);
  }
} // namespace p3109
