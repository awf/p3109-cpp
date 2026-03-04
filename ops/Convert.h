#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  // Convert<FormatR>(x, projSpec) -> r
  //
  // Convert a P3109 value to another P3109 format (main.tex §Convert).
  //
  // Behavior:
  //   Convert(NaN) -> NaN
  //   Convert(x)   -> Project<FormatR, projSpec>(Decode<FormatX>(x))
  template <typename FormatR, typename FormatX, typename ProjSpec>
  FormatR Convert(FormatX x, ProjSpec projSpec)
  {
    if (x.isnan())
      return FormatR::nan;
    return Project<FormatR>(Decode<FormatX>(x), projSpec);
  }
} // namespace p3109
