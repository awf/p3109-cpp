#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  template <typename FormatR, typename FormatX, typename ProjSpec>
  FormatR ArcTanPi(FormatX x, ProjSpec projSpec)
  {
    if (x.isnan())
      return FormatR::nan;
    if (x.isposinf())
      return Project<FormatR>(mpfr_half(), projSpec);
    if (x.isneginf())
      return Project<FormatR>(-mpfr_half(), projSpec);
    mpfr_float decoded = Decode<FormatX>(x);
    return Project<FormatR>(atan(decoded) / mpfr_pi(), projSpec);
  }
} // namespace p3109
