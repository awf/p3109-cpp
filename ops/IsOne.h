#pragma once

#include "ops/Decode.h"
#include "p3109.h"

namespace p3109 {
  template <typename FormatX>
  bool IsOne(FormatX x)
  {
    if (x.isnan())
      return false;
    if (x.isinf())
      return false;
    return Decode<FormatX>(x) == 1;
  }
} // namespace p3109
