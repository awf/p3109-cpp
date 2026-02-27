#pragma once

#include <boost/multiprecision/mpfr.hpp>
#include <cstdint>
#include <limits>
#include <string>
#include <type_traits>

namespace p3109 {
  constexpr std::uint64_t pow2_u64(unsigned n) { return std::uint64_t{1} << n; }
} // namespace p3109

namespace p3109 {

  enum Signedness { Signed, Unsigned };

  enum Domain { Finite, Extended };

  struct RoundingMode {};
  struct TowardZero : RoundingMode {};
  struct TowardMaxMagnitude : RoundingMode {};
  struct TowardPositive : RoundingMode {};
  struct TowardNegative : RoundingMode {};
  struct NearestTiesToAway : RoundingMode {};
  struct NearestTiesToEven : RoundingMode {};
  struct ToOdd : RoundingMode {};

  enum class StochasticVariant { A, B, C };

  template <StochasticVariant Variant, unsigned N>
  struct Stochastic : RoundingMode {
    static constexpr StochasticVariant variant = Variant;
    static constexpr unsigned bits = N;
    std::uint64_t random;
    explicit constexpr Stochastic(std::uint64_t r = 0) : random(r) {}
  };

  template <unsigned N>
  using StochasticA = Stochastic<StochasticVariant::A, N>;
  template <unsigned N>
  using StochasticB = Stochastic<StochasticVariant::B, N>;
  template <unsigned N>
  using StochasticC = Stochastic<StochasticVariant::C, N>;

  enum class SaturationMode {
    SatFinite,
    SatPropagate,
    OvfInf,
  };

  template <typename RM>
  struct ProjectionSpec {
    RM round;
    SaturationMode saturate;
  };

  template <unsigned K, unsigned P, Signedness Sigma, Domain Delta>
  struct binary {
    static_assert(3 <= K, "K must be at least 3");
    static_assert(1 <= P, "P must be at least 1");
    static_assert(
      P <= (Sigma == Signed ? (K - 1) : K), "P must be < K for signed formats, and <= K for unsigned formats");
    static_assert(K < 63, "K must be < 63 for this uint64_t reference implementation");

    std::uint64_t codepoint;

    static constexpr unsigned bitwidth = K;
    static constexpr unsigned precision = P;
    static constexpr Signedness signedness = Sigma;
    static constexpr Domain domain = Delta;
    static const unsigned int exponent_bias = (Sigma == Signed) ? pow2_u64(K - P - 1) : pow2_u64(K - P);

    static constexpr std::uint64_t nan_codepoint = (Sigma == Signed) ? pow2_u64(K - 1) : pow2_u64(K) - 1;
    static inline const binary nan = binary{nan_codepoint};

    static constexpr std::uint64_t posinf_codepoint = (Sigma == Signed) ? (pow2_u64(K - 1) - 1) : (pow2_u64(K) - 2);
    static inline const binary inf = binary{posinf_codepoint};

    static constexpr std::uint64_t neginf_codepoint = (Sigma == Signed) ? (pow2_u64(K) - 1) : 0;
    static inline const binary ninf = binary{neginf_codepoint};

    static constexpr bool isnan(const binary &x) { return x.codepoint == nan_codepoint; }
    static constexpr bool isinf(const binary &x)
    {
      if constexpr (Sigma == Signed)
        return x.codepoint == posinf_codepoint || x.codepoint == neginf_codepoint;
      else
        return x.codepoint == posinf_codepoint;
    }
    static constexpr bool isposinf(const binary &x) { return x.codepoint == posinf_codepoint; }
    static constexpr bool isneginf(const binary &x)
    {
      if constexpr (Sigma == Signed)
        return x.codepoint == neginf_codepoint;
      else
        return false;
    }

    constexpr bool isnan() const { return isnan(*this); }
    constexpr bool isinf() const { return isinf(*this); }
    constexpr bool isposinf() const { return isposinf(*this); }
    constexpr bool isneginf() const { return isneginf(*this); }

    static std::string name()
    {
      std::string out = "binary";
      out += std::to_string(K);
      out += "p";
      out += std::to_string(P);
      out += (Sigma == Signed ? "s" : "u");
      out += (Delta == Finite ? "f" : "x");
      return out;
    }
  };

  using mpfr_float = boost::multiprecision::mpfr_float;
  const mpfr_float mpfr_nan = std::numeric_limits<mpfr_float>::quiet_NaN();
  const mpfr_float mpfr_inf = std::numeric_limits<mpfr_float>::infinity();
  const mpfr_float mpfr_half = mpfr_float("0.5");

  inline void ensure_mpfr_precision()
  {
    static const bool configured = []() {
      constexpr unsigned kDecodePrecisionBits = 256;
      mpfr_float::default_precision(kDecodePrecisionBits);
      return true;
    }();
    (void)configured;
  }

} // namespace p3109

#include "ops/Decode.h"
#include "ops/Encode.h"
#include "ops/RoundToPrecision.h"
#include "ops/Saturate.h"
#include "ops/Project.h"
#include "ops/Sqrt.h"
