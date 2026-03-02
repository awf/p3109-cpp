#pragma once

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

  template <typename Format, typename RoundingMode, typename SM>
    requires(Format::is_extended || std::is_same_v<SM, SatFinite>)
  Format Project(mpfr_float X, ProjectionSpec<RoundingMode, SM> projectionSpec)
  {
    static_assert(std::is_base_of_v<p3109::RoundingMode, RoundingMode>, "RM must derive from RoundingMode");
    static_assert(std::is_base_of_v<p3109::SaturationMode, SM>, "SM must derive from SaturationMode");

    if (boost::math::isnan(X))
      return Encode<Format>(X);

    constexpr int bias = Format::exponent_bias;
    const mpfr_float max_finite = Decode(MaxFiniteOf<Format>());
    const mpfr_float rounded = RoundToPrecision<Format::precision, bias, RoundingMode>(X, projectionSpec.round);
    const mpfr_float saturated =
      Saturate<Format::signedness, Format::domain, SM, RoundingMode>(rounded, max_finite, projectionSpec.round);
    return Encode<Format>(saturated);
  }
} // namespace p3109
