#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  // FAA (Fused Add-Add): X + Y + Z
  template <typename FormatR, typename FormatX, typename FormatY, typename FormatZ, typename ProjSpec>
  FormatR FAA(FormatX x, FormatY y, FormatZ z, ProjSpec projSpec)
  {
    if (x.isnan())
      return FormatR::nan;
    if (y.isnan())
      return FormatR::nan;
    if (z.isnan())
      return FormatR::nan;
    // Opposing inf pairs → NaN
    if (x.isposinf() && y.isneginf())
      return FormatR::nan;
    if (x.isneginf() && y.isposinf())
      return FormatR::nan;
    if (x.isposinf() && z.isneginf())
      return FormatR::nan;
    if (x.isneginf() && z.isposinf())
      return FormatR::nan;
    if (y.isposinf() && z.isneginf())
      return FormatR::nan;
    if (y.isneginf() && z.isposinf())
      return FormatR::nan;
    // Any +inf propagates
    if (x.isposinf() || y.isposinf() || z.isposinf())
      return FormatR::inf;
    // Any -inf propagates
    if (x.isneginf() || y.isneginf() || z.isneginf())
      return FormatR::ninf;
    // All finite
    mpfr_float dx = Decode<FormatX>(x);
    mpfr_float dy = Decode<FormatY>(y);
    mpfr_float dz = Decode<FormatZ>(z);
    return Project<FormatR>(dx + dy + dz, projSpec);
  }
} // namespace p3109
