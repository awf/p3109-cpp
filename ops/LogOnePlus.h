#pragma once

#include "ops/Add.h"
#include "ops/Log.h"
#include "p3109.h"

namespace p3109 {
  // LogOnePlus(x) = Log(Add(1, x)) — composite operation
  template <typename FormatR, typename FormatX, typename ProjSpec>
  FormatR LogOnePlus(FormatX x, ProjSpec projSpec)
  {
    auto one = Encode<FormatR>(mpfr_float(1));
    auto sum = Add<FormatR>(one, x, projSpec);
    return Log<FormatR>(sum, projSpec);
  }
} // namespace p3109
