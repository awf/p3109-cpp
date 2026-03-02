#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  template <typename FormatR, typename FormatX, typename FormatY, typename ProjSpec>
  FormatR Multiply(FormatX x, FormatY y, ProjSpec projSpec)
  {
    if (x.isnan())
      return FormatR::nan;
    if (y.isnan())
      return FormatR::nan;

    mpfr_float dx = Decode<FormatX>(x);
    mpfr_float dy = Decode<FormatY>(y);

    // inf * inf: sign(x)*sign(y) determines result sign
    if (x.isinf() && y.isinf())
    {
      bool same_sign = (dx > 0) == (dy > 0);
      return same_sign ? FormatR::inf : FormatR::ninf;
    }
    // inf * 0 = NaN, 0 * inf = NaN
    if (x.isinf() && dy == 0)
      return FormatR::nan;
    if (y.isinf() && dx == 0)
      return FormatR::nan;
    // inf * finite: sign determines result
    if (x.isinf())
      return (dy > 0) ? (dx > 0 ? FormatR::inf : FormatR::ninf) : (dx > 0 ? FormatR::ninf : FormatR::inf);
    if (y.isinf())
      return (dx > 0) ? (dy > 0 ? FormatR::inf : FormatR::ninf) : (dy > 0 ? FormatR::ninf : FormatR::inf);

    return Project<FormatR>(dx * dy, projSpec);
  }
} // namespace p3109
