#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  template <typename FormatR, typename FormatX, typename FormatY, typename ProjSpec>
  FormatR Maximum(FormatX x, FormatY y, ProjSpec projSpec)
  {
    if (x.isnan())
      return FormatR::nan;
    if (y.isnan())
      return FormatR::nan;
    if (x.isposinf() || y.isposinf())
      return FormatR::inf;
    if (x.isneginf())
    {
      if (y.isneginf())
        return FormatR::ninf;
      return Project<FormatR>(Decode<FormatY>(y), projSpec);
    }
    if (y.isneginf())
      return Project<FormatR>(Decode<FormatX>(x), projSpec);
    mpfr_float dx = Decode<FormatX>(x);
    mpfr_float dy = Decode<FormatY>(y);
    return Project<FormatR>(dx < dy ? dy : dx, projSpec);
  }
} // namespace p3109
