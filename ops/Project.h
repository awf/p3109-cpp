#pragma once

#include <stdexcept>
#include <type_traits>

#include "ops/Format.h"
#include "p3109.h"

namespace p3109 {
  // Project<Format, projspec>(X) -> x
  //
  // Structured definition alignment (main.tex §Project):
  // - Parameters:
  //     Format   = target format type (e.g., binary<K, P, Sigma, Delta>)
  //     projspec = projection specification (round, sat)
  // - Operand:
  //     X = closed extended real value
  // - Result:
  //     x = P3109 value in format Format
  //
  // Behavior mapping used below:
  //   R = RoundToPrecision(X)
  //   S = Saturate(R, MaxFinite(Format), sat, round)
  //   x = Encode(S)

  template <typename Format, typename RoundingMode = NearestTiesToEven>
  Format Project(mpfr_float X, SaturationMode sat, RoundingMode roundMode = RoundingMode{})
  {
    static_assert(std::is_base_of_v<RoundingMode, RoundingMode>, "RM must derive from RoundingMode");

    if constexpr (Format::domain == Finite)
    {
      if (sat != SatFinite)
        throw std::invalid_argument("Finite-domain Project requires SatFinite");
    }

    if (boost::math::isnan(X))
      return Encode<Format>(X);

    constexpr int bias = Format::exponent_bias;
    const mpfr_float max_finite = Decode(MaxFiniteOf<Format>());
    const mpfr_float rounded = RoundToPrecision<Format::precision, bias, RoundingMode>(X, roundMode);
    const mpfr_float saturated =
      Saturate<Format::signedness, Format::domain, RoundingMode>(rounded, max_finite, sat, roundMode);
    return Encode<Format>(saturated);
  }

  template <typename Format, typename RoundingMode, SaturationMode Sat>
  Format Project(mpfr_float X, ProjectionSpec<RoundingMode, Sat> projectionSpec)
  {
    return Project<Format, RoundingMode>(X, Sat, projectionSpec.round);
  }
} // namespace p3109
