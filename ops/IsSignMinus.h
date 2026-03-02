#pragma once

#include "ops/Decode.h"
#include "p3109.h"

namespace p3109 {
  template <typename FormatX>
  bool IsSignMinus(FormatX x)
  {
    if (x.isnan())
      return false;
    if (x.isposinf())
      return false;
    if (x.isneginf())
      return true;
    return Decode<FormatX>(x) < 0;
  }
} // namespace p3109
