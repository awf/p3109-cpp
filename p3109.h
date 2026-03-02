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

  enum class Signedness { Signed, Unsigned };

  enum class Domain { Finite, Extended };

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
    constexpr Stochastic(std::uint64_t r = 0) : random(r) {}
  };

  template <unsigned N>
  using StochasticA = Stochastic<StochasticVariant::A, N>;
  template <unsigned N>
  using StochasticB = Stochastic<StochasticVariant::B, N>;
  template <unsigned N>
  using StochasticC = Stochastic<StochasticVariant::C, N>;

  struct SaturationMode {};
  struct SatFinite : SaturationMode {};
  struct SatPropagate : SaturationMode {};
  struct OvfInf : SaturationMode {};

  template <typename RoundingMode, typename SaturationMode>
  struct ProjectionSpec {
    static_assert(std::is_base_of_v<p3109::SaturationMode, SaturationMode>, "SM must derive from SaturationMode");
    RoundingMode round;
  };

  template <unsigned K, unsigned P, Signedness Sigma, Domain Delta>
  struct binary {
    static constexpr bool is_signed = (Sigma == Signedness::Signed);
    static constexpr bool is_extended = (Delta == Domain::Extended);

    static_assert(3 <= K, "K must be at least 3");
    static_assert(1 <= P, "P must be at least 1");
    static_assert(P <= (is_signed ? (K - 1) : K), "P must be < K for signed formats, and <= K for unsigned formats");
    static_assert(K < 63, "K must be < 63 for this uint64_t reference implementation");

    std::uint64_t codepoint;

    static constexpr unsigned bitwidth = K;
    static constexpr unsigned precision = P;
    static constexpr Signedness signedness = Sigma;
    static constexpr Domain domain = Delta;

    static const unsigned int exponent_bias = is_signed ? pow2_u64(K - P - 1) : pow2_u64(K - P);

    static constexpr std::uint64_t nan_codepoint = is_signed ? pow2_u64(K - 1) : pow2_u64(K) - 1;
    static inline const binary nan = binary{nan_codepoint};

    static constexpr std::uint64_t posinf_codepoint = is_signed ? (pow2_u64(K - 1) - 1) : (pow2_u64(K) - 2);
    static inline const binary inf = binary{posinf_codepoint};

    static constexpr std::uint64_t neginf_codepoint = is_signed ? (pow2_u64(K) - 1) : 0;
    static inline const binary ninf = binary{neginf_codepoint};

    static constexpr bool isnan(const binary &x) { return x.codepoint == nan_codepoint; }
    static constexpr bool isinf(const binary &x)
    {
      if constexpr (is_signed)
        return x.codepoint == posinf_codepoint || x.codepoint == neginf_codepoint;
      else
        return x.codepoint == posinf_codepoint;
    }
    static constexpr bool isposinf(const binary &x) { return x.codepoint == posinf_codepoint; }
    static constexpr bool isneginf(const binary &x)
    {
      if constexpr (is_signed)
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
      out += (is_signed ? "s" : "u");
      out += (is_extended ? "e" : "f");
      return out;
    }
  };

  using mpfr_float = boost::multiprecision::mpfr_float;

  inline void ensure_mpfr_precision()
  {
    static const bool configured = []() {
      constexpr unsigned kDecodePrecisionBits = 256;
      mpfr_float::default_precision(kDecodePrecisionBits);
      return true;
    }();
    (void)configured;
  }

  // These are functions (not globals) to avoid static-init-order issues:
  // each ensures MPFR precision is configured before the value is first created.
  inline const mpfr_float &mpfr_nan()
  {
    ensure_mpfr_precision();
    static const mpfr_float val = std::numeric_limits<mpfr_float>::quiet_NaN();
    return val;
  }
  inline const mpfr_float &mpfr_inf()
  {
    ensure_mpfr_precision();
    static const mpfr_float val = std::numeric_limits<mpfr_float>::infinity();
    return val;
  }
  inline const mpfr_float &mpfr_half()
  {
    ensure_mpfr_precision();
    static const mpfr_float val = mpfr_float("0.5");
    return val;
  }

} // namespace p3109

#include "ops/Decode.h"
#include "ops/Encode.h"
#include "ops/RoundToPrecision.h"
#include "ops/Saturate.h"
#include "ops/Project.h"
#include "ops/Abs.h"
#include "ops/Add.h"
#include "ops/CompareEqual.h"
#include "ops/FMA.h"
#include "ops/IsZero.h"
#include "ops/Sqrt.h"
