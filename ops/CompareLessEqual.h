#pragma once

#include "ops/Decode.h"
#include "p3109.h"

namespace p3109 {
  template <typename FormatX, typename FormatY>
  bool CompareLessEqual(FormatX x, FormatY y)
  {
    if (x.isnan())
      return false;
    if (y.isnan())
      return false;
    if (y.isposinf())
      return true;
    if (x.isneginf())
      return true;
    if (x.isposinf())
      return false;
    if (y.isneginf())
      return false;
    return Decode<FormatX>(x) <= Decode<FormatY>(y);
  }
} // namespace p3109
