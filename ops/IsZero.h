#pragma once

#include "ops/Decode.h"
#include "p3109.h"

namespace p3109 {
  // IsZero(x) -> bool
  //
  // Structured definition (main.tex §IsZero):
  // - Parameters:
  //     FormatX   = format of operand x (deduced from x)
  // - Operand:
  //     x         = P3109 value, format FormatX
  // - Result:
  //     bool
  //
  // Note: Predicates do not take a ProjectionSpec.
  //
  template <typename FormatX>
  bool IsZero(FormatX x)
  {
    // Canonical IsZero cases (from wIsZero.tex):
    // 1. IsZero(NaN) -> false
    if (x.isnan())
      return false;
    // 2. IsZero(±inf) -> false
    if (x.isinf())
      return false;
    // 3. IsZero(X) -> X == 0
    return Decode<FormatX>(x) == 0;
  }
} // namespace p3109
