#pragma once

#include "ops/Decode.h"
#include "ops/Minimum.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  // MinimumNumber: NaN-skipping variant of Minimum
  template <typename FormatR, typename FormatX, typename FormatY, typename ProjSpec>
  FormatR MinimumNumber(FormatX x, FormatY y, ProjSpec projSpec)
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
    return Minimum<FormatR>(x, y, projSpec);
  }
} // namespace p3109
