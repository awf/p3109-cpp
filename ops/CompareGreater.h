#pragma once

#include "ops/Decode.h"
#include "p3109.h"

namespace p3109 {
  template <typename FormatX, typename FormatY>
  bool CompareGreater(FormatX x, FormatY y)
  {
    // CompareGreater(NaN, *) -> False
    if (x.isnan())
      return false;
    // CompareGreater(*, NaN) -> False
    if (y.isnan())
      return false;
    // CompareGreater(*, +inf) -> False
    if (y.isposinf())
      return false;
    // CompareGreater(-inf, -inf) -> False
    if (x.isneginf() && y.isneginf())
      return false;
    // CompareGreater(X, -inf) -> True  (X != -inf)
    if (y.isneginf())
      return true;
    // CompareGreater(+inf, Y) -> True  (Y != +inf)
    if (x.isposinf())
      return true;
    // CompareGreater(-inf, Y) -> False  (Y != -inf)
    if (x.isneginf())
      return false;
    return Decode<FormatX>(x) > Decode<FormatY>(y);
  }
} // namespace p3109
