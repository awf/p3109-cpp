#pragma once

#include <type_traits>

#include <boost/math/special_functions/fpclassify.hpp>

#include "p3109.h"

namespace p3109
{
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
    namespace detail
    {
        template <typename RM>
        inline bool ovf_to_inf_positive(RM)
        {
            if constexpr (std::is_same_v<RM, TowardZero> || std::is_same_v<RM, TowardNegative>)
                return false;
            if constexpr (std::is_base_of_v<RoundingMode, RM>)
                return true;
            return true;
        }

        template <typename RM>
        inline bool ovf_to_inf_negative(RM)
        {
            if constexpr (std::is_same_v<RM, TowardZero> || std::is_same_v<RM, TowardPositive>)
                return false;
            if constexpr (std::is_base_of_v<RoundingMode, RM>)
                return true;
            return true;
        }
    }

    template <Signedness Sigma, Domain Delta, typename RM>
    mpfr_float Saturate(mpfr_float X,
                        mpfr_float maxFinite,
                        SaturationMode sat,
                        RM roundMode = RM{})
    {
        static_assert(std::is_base_of_v<RoundingMode, RM>, "RM must derive from RoundingMode");

        ensure_mpfr_precision();

        if (boost::math::isnan(X))
            return X;

        const mpfr_float minFinite = (Sigma == Signed) ? -maxFinite : mpfr_float(0.0);

        const auto clamp_finite = [&]() -> mpfr_float
        {
            if (X > maxFinite)
                return maxFinite;
            if (X < minFinite)
                return minFinite;
            return X;
        };

        switch (sat)
        {
        case SaturationMode::SatFinite:
            if (boost::math::isinf(X))
                return (X > 0) ? maxFinite : minFinite;
            return clamp_finite();

        case SaturationMode::SatPropagate:
            if (boost::math::isinf(X))
            {
                if constexpr (Sigma == Unsigned)
                    return (X > 0) ? mpfr_inf : mpfr_float(0.0);
                return X;
            }
            return clamp_finite();

        case SaturationMode::OvfInf:
            if (boost::math::isinf(X))
            {
                if (X > 0)
                    return mpfr_inf;
                if constexpr (Sigma == Signed)
                    return -mpfr_inf;
                return mpfr_float(0.0);
            }

            if (X > maxFinite)
                return detail::ovf_to_inf_positive(roundMode) ? mpfr_inf : maxFinite;

            if (X < minFinite)
            {
                if constexpr (Sigma == Unsigned)
                    return mpfr_float(0.0);
                return detail::ovf_to_inf_negative(roundMode) ? -mpfr_inf : minFinite;
            }

            return X;
        }

        return X;
    }
} // namespace p3109
