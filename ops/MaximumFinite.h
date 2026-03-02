#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  // MaximumFinite: NaN-ignoring, inf-ignoring (prefers finite values)
  template <typename FormatR, typename FormatX, typename FormatY, typename ProjSpec>
  FormatR MaximumFinite(FormatX x, FormatY y, ProjSpec projSpec)
  {
    if (x.isnan() && y.isnan())
      return FormatR::nan;
    if (x.isnan())
    {
      if (y.isposinf())
        return FormatR::inf;
      if (y.isneginf())
        return FormatR::ninf;
      return Project<FormatR>(Decode<FormatY>(y), projSpec);
    }
    if (y.isnan())
    {
      if (x.isposinf())
        return FormatR::inf;
      if (x.isneginf())
        return FormatR::ninf;
      return Project<FormatR>(Decode<FormatX>(x), projSpec);
    }
    // Both inf
    if (x.isinf() && y.isinf())
    {
      if (x.isposinf() || y.isposinf())
        return FormatR::inf;
      return FormatR::ninf;
    }
    // One inf: return the other (finite) operand
    if (x.isinf())
    {
      if (y.isneginf())
        return FormatR::ninf;
      return Project<FormatR>(Decode<FormatY>(y), projSpec);
    }
    if (y.isinf())
    {
      if (x.isneginf())
        return FormatR::ninf;
      return Project<FormatR>(Decode<FormatX>(x), projSpec);
    }
    mpfr_float dx = Decode<FormatX>(x);
    mpfr_float dy = Decode<FormatY>(y);
    return Project<FormatR>(dx < dy ? dy : dx, projSpec);
  }
} // namespace p3109
