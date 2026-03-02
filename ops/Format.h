#pragma once

#include "p3109.h"
#include <cstdint>

namespace p3109 {
  // MaxFiniteOf<Format>: Returns the Format value with the maximum finite codepoint for a given format.
  // (Spec-aligned: see main.tex for full set of format-derived constants.)
  template <typename Format>
  constexpr Format MaxFiniteOf()
  {
    constexpr unsigned K = Format::bitwidth;
    constexpr std::uint64_t two_to_k = pow2_u64(K);
    constexpr std::uint64_t two_to_km1 = pow2_u64(K - 1);

    std::uint64_t codepoint = 0;
    if constexpr (Format::is_signed)
    {
      if constexpr (Format::is_extended)
        codepoint = two_to_km1 - 2;
      else
        codepoint = two_to_km1 - 1;
    }
    else
    {
      if constexpr (Format::is_extended)
        codepoint = two_to_k - 3;
      else
        codepoint = two_to_k - 2;
    }
    return Format{codepoint};
  }
  // MinPositiveOf<Format>: Returns the Format value with the minimum positive codepoint for a given format.
  // This corresponds to codepoint 1, which encodes the smallest positive (subnormal) value.
  template <typename Format>
  constexpr Format MinPositiveOf()
  {
    return Format{1};
  }

  // MinFiniteOf<Format>: Returns the Format value with the minimum (most negative) finite codepoint.
  // For signed formats this is the reflection of MaxFiniteOf; for unsigned formats it is zero.
  template <typename Format>
  constexpr Format MinFiniteOf()
  {
    constexpr unsigned K = Format::bitwidth;
    constexpr std::uint64_t two_to_k = pow2_u64(K);

    if constexpr (!Format::is_signed)
      return Format{0};
    else if constexpr (Format::is_extended)
      return Format{two_to_k - 2};
    else
      return Format{two_to_k - 1};
  }

  // MinNormalOf<Format>: Returns the Format value with the minimum normal codepoint (2^(P-1)).
  template <typename Format>
  constexpr Format MinNormalOf()
  {
    return Format{pow2_u64(Format::precision - 1)};
  }

  // ExponentBitsOf<Format>: Returns the number of exponent bits in a given format.
  // (Spec-aligned: K-P-1 for signed formats, K-P for unsigned formats.)
  template <typename Format>
  constexpr unsigned ExponentBitsOf()
  {
    if constexpr (Format::is_signed)
      return Format::bitwidth - Format::precision - 1;
    else
      return Format::bitwidth - Format::precision;
  }

  // TrailingBitsOf<Format>: Returns the number of trailing significand bits in a given format (P-1).
  template <typename Format>
  constexpr unsigned TrailingBitsOf()
  {
    return Format::precision - 1;
  }

  // ExponentBiasOf<Format>: Returns the exponent bias for a given format.
  // (Spec-aligned: bias = 2^(K-P-1) for signed formats, 2^(K-P) for unsigned formats.)
  template <typename Format>
  constexpr unsigned ExponentBiasOf()
  {
    return Format::exponent_bias;
  }
} // namespace p3109
