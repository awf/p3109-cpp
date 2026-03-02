#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  template <typename FormatR, typename FormatX, typename FormatY, typename ProjSpec>
  FormatR Subtract(FormatX x, FormatY y, ProjSpec projSpec)
  {
    if (x.isnan())
      return FormatR::nan;
    if (y.isnan())
      return FormatR::nan;
    // +inf - +inf = NaN, -inf - -inf = NaN
    if (x.isposinf() && y.isposinf())
      return FormatR::nan;
    if (x.isneginf() && y.isneginf())
      return FormatR::nan;
    // X - +inf = -inf
    if (y.isposinf())
      return FormatR::ninf;
    // +inf - Y = +inf
    if (x.isposinf())
      return FormatR::inf;
    // X - -inf = +inf
    if (y.isneginf())
      return FormatR::inf;
    // -inf - Y = -inf
    if (x.isneginf())
      return FormatR::ninf;
    mpfr_float dx = Decode<FormatX>(x);
    mpfr_float dy = Decode<FormatY>(y);
    return Project<FormatR>(dx - dy, projSpec);
  }
} // namespace p3109
