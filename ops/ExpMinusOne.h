#pragma once

#include "ops/Exp.h"
#include "ops/Subtract.h"
#include "p3109.h"

namespace p3109 {
  // ExpMinusOne(x) = Subtract(Exp(x), 1) — composite operation
  template <typename FormatR, typename FormatX, typename ProjSpec>
  FormatR ExpMinusOne(FormatX x, ProjSpec projSpec)
  {
    auto exp_x = Exp<FormatR>(x, projSpec);
    auto one = Encode<FormatR>(mpfr_float(1));
    return Subtract<FormatR>(exp_x, one, projSpec);
  }
} // namespace p3109
