#pragma once

#include "p3109.h"

namespace p3109 {
  // Decode<Format>(x) -> X
  //
  // Structured definition alignment (main.tex §Decode):
  // - Parameters:
  //     Format = binary<K, P, Sigma, Delta>
  // - Operand:
  //     x = codepoint (unsigned integer)
  // - Result:
  //     X = real value (closed extended real)
  //
  // Behavior mapping:
  //   - Special codepoints for NaN, ±inf (see spec)
  //   - Signed reflection for negative values
  //   - Normal/subnormal decoding via trailing_modulus, exponent bias
  //
  // See main.tex for full details and edge case handling.
  template <typename Format>
  mpfr_float DecodeAux(Format x)
  {
    ensure_mpfr_precision();

    constexpr unsigned K = Format::bitwidth;
    constexpr unsigned P = Format::precision;
    constexpr std::uint64_t two_to_k = pow2_u64(K);
    constexpr std::uint64_t two_to_km1 = pow2_u64(K - 1);

    // NaN codepoints
    std::uint64_t codepoint = x.codepoint;
    if (Format::is_signed)
    {
      if (codepoint == two_to_km1)
        return mpfr_nan();
    }
    else
    {
      if (codepoint == (two_to_k - 1))
        return mpfr_nan();
    }

    // Infinity codepoints (Extended domain only)
    if constexpr (Format::is_extended)
    {
      if (Format::is_signed)
      {
        if (codepoint == (two_to_km1 - 1))
          return mpfr_inf();
        if (codepoint == (two_to_k - 1))
          return -mpfr_inf();
      }
      else
      {
        if (codepoint == (two_to_k - 2))
          return mpfr_inf();
      }
    }

    // Signed reflection rule:
    // DecodeAux(Signed, domain, x) = -DecodeAux(Signed, domain, x - 2^(K-1)) for 2^(K-1) < x < 2^K
    if constexpr (Format::is_signed)
    {
      if (codepoint > two_to_km1 && codepoint < two_to_k)
      {
        Format reflected = x;
        reflected.codepoint = codepoint - two_to_km1;
        return -DecodeAux<Format>(reflected);
      }
    }

    // Common normal/subnormal decode
    constexpr std::uint64_t trailing_modulus = pow2_u64(P - 1);
    constexpr int bias = Format::exponent_bias;

    const std::uint64_t T = codepoint % trailing_modulus;
    const std::uint64_t E = codepoint / trailing_modulus;

    if (E == 0)
    {
      // Subnormals
      // (0 + T * 2^(1-P)) * 2^(1-bias)
      mpfr_float out = T;
      out *= pow(mpfr_float(2.0), 2 - static_cast<int>(P) - bias);
      return out;
    }

    // (1 + T * 2^(1-P)) * 2^(E-bias)
    mpfr_float t_scaled = T;
    t_scaled *= pow(mpfr_float(2.0), 1 - static_cast<int>(P));

    mpfr_float significand = 1;
    significand += t_scaled;
    significand *= pow(mpfr_float(2.0), static_cast<long>(E) - bias);
    return significand;
  }

  template <typename Format>
  mpfr_float Decode(Format x)
  {
    return DecodeAux<Format>(x);
  }
} // namespace p3109
