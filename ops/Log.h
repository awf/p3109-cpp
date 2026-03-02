#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  template <typename FormatR, typename FormatX, typename ProjSpec>
  FormatR Log(FormatX x, ProjSpec projSpec)
  {
    if (x.isnan())
      return FormatR::nan;
    if (x.isneginf())
      return FormatR::nan;
    if (x.isposinf())
      return FormatR::inf;
    mpfr_float decoded = Decode<FormatX>(x);
    if (decoded < 0)
      return FormatR::nan;
    if (decoded == 0)
      return FormatR::ninf;
    return Project<FormatR>(log(decoded), projSpec);
  }
} // namespace p3109
