#pragma once

#include <cmath>

#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  // ConvertFromIEEE754<FormatR>(x, projSpec) -> r
  //
  // Convert an IEEE 754 value (float/double) to a P3109 format
  // (main.tex §ConvertFromIEEE754).
  //
  // Behavior:
  //   ConvertFromIEEE754(NaN)  -> NaN
  //   ConvertFromIEEE754(x)    -> Project<FormatR, projSpec>(AsClosedExtendedReal(x))
  //
  // AsClosedExtendedReal maps IEEE 754 values to closed extended reals:
  //   -0 -> 0, ±inf -> ±inf, finite -> value
  template <typename FormatR, typename IEEEType, typename ProjSpec>
  FormatR ConvertFromIEEE754(IEEEType x, ProjSpec projSpec)
  {
    ensure_mpfr_precision();

    if (std::isnan(x))
      return FormatR::nan;

    // AsClosedExtendedReal: -0 becomes 0 (no negative zero in closed extended reals)
    mpfr_float val(x);
    if (val == 0)
      val = mpfr_float(0);

    return Project<FormatR>(val, projSpec);
  }
} // namespace p3109
