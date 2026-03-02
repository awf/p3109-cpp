#pragma once

#include "ops/Decode.h"
#include "ops/Project.h"
#include "p3109.h"

namespace p3109 {
  // FMA<FormatR>(x, y, z, projSpec) -> r
  //
  // Structured definition (main.tex §FMA):
  // - Parameters:
  //     FormatR   = format of result r
  //     FormatX   = format of operand x (deduced)
  //     FormatY   = format of operand y (deduced)
  //     FormatZ   = format of operand z (deduced)
  //     ProjSpec  = projection specification type (ProjectionSpec<...>)
  // - Operands:
  //     x         = P3109 value, format FormatX
  //     y         = P3109 value, format FormatY
  //     z         = P3109 value, format FormatZ
  //     projSpec  = ProjSpec (projection specification)
  // - Result:
  //     r         = P3109 value, format FormatR
  //
  template <typename FormatR, typename FormatX, typename FormatY, typename FormatZ, typename ProjSpec>
  FormatR FMA(FormatX x, FormatY y, FormatZ z, ProjSpec projSpec)
  {
    // Canonical FMA cases (from wFMA.tex):
    // NaN propagation
    // 1. FMA(NaN, any, any) -> NaN
    if (x.isnan())
      return FormatR::nan;
    // 2. FMA(any, NaN, any) -> NaN
    if (y.isnan())
      return FormatR::nan;
    // 3. FMA(any, any, NaN) -> NaN
    if (z.isnan())
      return FormatR::nan;

    mpfr_float dx = Decode<FormatX>(x);
    mpfr_float dy = Decode<FormatY>(y);
    mpfr_float dz = Decode<FormatZ>(z);

    const bool x_inf = x.isinf();
    const bool y_inf = y.isinf();
    const bool z_inf = z.isinf();

    // 4. FMA(0, ±inf, any) -> NaN
    if (dx == 0 && y_inf)
      return FormatR::nan;
    // 5. FMA(±inf, 0, any) -> NaN
    if (x_inf && dy == 0)
      return FormatR::nan;

    // inf*inf + opposite-sign inf -> NaN (cases 6-17 in wFMA.tex)
    // When both product and addend are infinite, result is NaN if they oppose.
    if ((x_inf || y_inf) && z_inf)
    {
      // Product sign: sign(x)*sign(y)
      bool prod_pos = (dx > 0) == (dy > 0);
      bool z_pos = dz > 0;
      if (prod_pos != z_pos)
        return FormatR::nan;
      // Same sign: result is that infinity
      return z_pos ? FormatR::inf : FormatR::ninf;
    }

    // Infinite product, finite z
    if (x_inf || y_inf)
    {
      bool prod_pos = (dx > 0) == (dy > 0);
      return prod_pos ? FormatR::inf : FormatR::ninf;
    }

    // Finite product, infinite z
    if (z_inf)
      return dz > 0 ? FormatR::inf : FormatR::ninf;

    // All finite: FMA(X, Y, Z) -> X * Y + Z
    mpfr_float result = dx * dy + dz;
    return Project<FormatR>(result, projSpec);
  }
} // namespace p3109
