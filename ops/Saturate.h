#pragma once

#include <stdexcept>
#include <string>
#include <type_traits>

#include <boost/math/special_functions/fpclassify.hpp>

#include "p3109.h"

namespace p3109 {
  // Saturate<Sigma, Delta, RM>(X, maxFinite, sat, roundMode) -> Z
  //
  // Structured definition alignment (main.tex §Saturate):
  // - Parameters:
  //     Sigma    = signedness (Signed/Unsigned)
  //     Delta    = domain (Finite/Extended)
  //     RM       = rounding mode policy type
  // - Operands:
  //     X         = real value (already rounded)
  //     maxFinite = maximum finite value for the format (Format or decoded value)
  //     sat       = saturation mode (SatFinite, SatPropagate, OvfInf)
  //     roundMode = rounding mode policy instance (for OvfInf branch)
  // - Result:
  //     Z = saturated real value (possibly ±inf, maxFinite, minFinite, or X)
  //
  // Behavior mapping:
  //   - If sat == SatFinite: clamp to [minFinite, maxFinite], inf → min/max
  //   - If sat == SatPropagate: propagate ±inf, clamp finite, unsigned: -inf→0
  //   - If sat == OvfInf: overflow to ±inf or clamp, depending on RM and Sigma
  //
  // See main.tex for full details and edge case handling.

  template <Signedness Sigma, Domain Delta, typename RoundingMode>
  mpfr_float Saturate(mpfr_float X, mpfr_float maxFinite, SaturationMode sat, RoundingMode roundMode = RoundingMode{})
  {
    static_assert(std::is_base_of_v<p3109::RoundingMode, RoundingMode>, "RM must derive from RoundingMode");
    constexpr bool is_signed = (Sigma == Signedness::Signed);
    constexpr bool is_extended = (Delta == Domain::Extended);

    //   M^lo = -MaxFinite if σ = Signed, else 0
    //   M^hi = MaxFinite
    const mpfr_float Mlo = is_signed ? -maxFinite : mpfr_float(0.0);
    const mpfr_float Mhi = maxFinite;

    const bool is_finite = boost::math::isfinite(X);
    const bool is_pinf = !is_finite && (X > 0);
    const bool is_ninf = !is_finite && (X < 0);

    // ωSaturate(*, *, NaN, *, *) -> NaN
    if (boost::math::isnan(X))
      return X;

    // ωSaturate(*, *, X, *, *) if M^lo <= X and X <= M^hi -> X
    if (Mlo <= X && X <= Mhi)
      return X;

    if (sat == SaturationMode::SatFinite)
    {
      // ωSaturate(SatFinite, *, +∞, *, *) -> M^hi
      if (is_pinf)
        return Mhi;

      // ωSaturate(SatFinite, *, −∞, *, *) -> M^lo
      if (is_ninf)
        return Mlo;

      // ωSaturate(SatFinite, *, X, *, *) if X <= M^lo -> M^lo
      if (X <= Mlo)
        return Mlo;

      // ωSaturate(SatFinite, *, X, *, *) if X >= M^hi -> M^hi
      if (X >= Mhi)
        return Mhi;
    }
    else if (sat == SaturationMode::SatPropagate)
    {
      // ωSaturate(SatPropagate, *, +∞, *, Extended) -> +∞
      if (is_pinf && is_extended)
        return mpfr_inf();

      // ωSaturate(SatPropagate, *, +∞, *, *) -> M^hi
      if (is_pinf)
        return Mhi;

      // ωSaturate(SatPropagate, *, −∞, Signed, Extended) -> −∞
      if (is_ninf && is_signed && is_extended)
        return -mpfr_inf();

      // ωSaturate(SatPropagate, *, −∞, *, *) -> M^lo
      if (is_ninf)
        return Mlo;

      // ωSaturate(SatPropagate, *, X, *, *) if X <= M^lo -> M^lo
      if (X <= Mlo)
        return Mlo;

      // ωSaturate(SatPropagate, *, X, *, *) if X >= M^hi -> M^hi
      if (X >= Mhi)
        return Mhi;
    }
    else if (sat == SaturationMode::OvfInf)
    {
      // ωSaturate(OvfInf, *, +∞, *, Extended) -> +∞
      if (is_pinf && is_extended)
        return mpfr_inf();

      // ωSaturate(OvfInf, *, +∞, *, *) -> M^hi
      if (is_pinf)
        return Mhi;

      // ωSaturate(OvfInf, *, −∞, Signed, Extended) -> −∞
      if (is_ninf && is_signed && is_extended)
        return -mpfr_inf();

      // ωSaturate(OvfInf, *, −∞, *, *) -> M^lo
      if (is_ninf)
        return Mlo;

      // ωSaturate(OvfInf, TowardZero ∨ TowardPositive, X, *, *) if X <= M^lo -> M^lo
      if ((std::is_same_v<RoundingMode, TowardZero> || std::is_same_v<RoundingMode, TowardPositive>) && X <= Mlo)
        return Mlo;

      // ωSaturate(OvfInf, TowardZero ∨ TowardNegative, X, *, *) if X >= M^hi -> M^hi
      if ((std::is_same_v<RoundingMode, TowardZero> || std::is_same_v<RoundingMode, TowardNegative>) && X >= Mhi)
        return Mhi;

      // ωSaturate(OvfInf, ToOdd, X, Unsigned, *) if X >= M^hi -> M^hi
      if (!is_signed && std::is_same_v<RoundingMode, ToOdd>)
      {
        if (X >= Mhi)
          return Mhi;
      }

      // ωSaturate(OvfInf, *, X, Signed, Extended) if X <= M^lo -> −∞
      if (X <= Mlo && is_signed && is_extended)
        return -mpfr_inf();

      // ωSaturate(OvfInf, *, X, *, *) if X <= M^lo -> M^lo
      if (X <= Mlo)
        return Mlo;

      // ωSaturate(OvfInf, *, X, *, Extended) if X >= M^hi -> +∞
      if (X >= Mhi && is_extended)
        return mpfr_inf();

      // ωSaturate(OvfInf, *, X, *, *) if X >= M^hi -> M^hi
      if (X >= Mhi)
        return Mhi;
    }
    throw std::logic_error("Saturate: unhandled case (sat=" + std::to_string(static_cast<int>(sat)) + ")");
  }
} // namespace p3109
