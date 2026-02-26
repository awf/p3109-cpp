#pragma once

#include <algorithm>
#include <cstdint>
#include <type_traits>

#include <boost/math/special_functions/fpclassify.hpp>

#include "p3109.h"

namespace p3109
{
    namespace detail
    {
        // Helper used by CodeIsEven and RNITE in the RoundToPrecision definition.
        inline bool is_even_i64(long long v)
        {
            return (v & 1LL) == 0;
        }

        // RNITE(X): round-to-nearest-integer, ties-to-even.
        // Spec mapping: RNITE in main.tex RoundToPrecision definition.
        inline long long rnite_ties_to_even(const mpfr_float &x)
        {
            const long long f = floor(x).convert_to<long long>();
            const mpfr_float half = mpfr_float(f) + mpfr_half;
            if ((x < half) || (x == half && is_even_i64(f)))
                return f;
            return f + 1;
        }
    }

    // RoundToPrecision<P, Bias>(X, RM{}) -> Z
    //
    // Structured definition alignment (main.tex §RoundToPrecision):
    // - Signature:
    //     RoundToPrecision<p, b, RM>(X) gives Z
    // - Parameters:
    //     p    = precision (template parameter P)
    //     b    = exponent bias (template parameter Bias)
    //     RM   = rounding mode policy type (TowardZero, NearestTiesToEven,
    //            StochasticA<N>, StochasticB<N>, StochasticC<N>)
    //     stochastic variants additionally use mode parameters:
    //       N   = number of random bits (template argument in StochasticA<N>/B/C)
    //       R   = random integer 0 <= R < 2^N (mode.random)
    // - Operand:
    //     X = real value (closed extended reals handled pass-through for 0/±inf/NaN)
    // - Result:
    //     Z = sign(X) * I * 2^E, representable at precision P with lower exponent bound
    //         induced by Bias via E = max(floor(log2(|X|)), 1-Bias) - P + 1
    //
    // Variable mapping to spec notation:
    //   E         <-> E
    //   S         <-> S (real-valued significand)
    //   floor_s   <-> floor(S)
    //   fraction  <-> eta = S - floor(S)
    //   code_is_even <-> CodeIsEven
    //   round_away   <-> RoundAway(RM)
    //   I         <-> I
    //
    // Notes:
    // - Subnormal handling follows the spec via max(floor(log2(|X|)), 1-Bias).
    // - StochasticA/B/C are implemented exactly from the listed inequalities.
    template <unsigned P, int Bias, typename RM>
    mpfr_float RoundToPrecision(mpfr_float X, RM mode = RM{})
    {
        static_assert(std::is_base_of_v<RoundingMode, RM>, "RM must derive from RoundingMode");

        ensure_mpfr_precision();

        if (X == 0 || boost::math::isinf(X))
            return X;

        if (boost::math::isnan(X))
            return X;

        // E = max(floor(log2(|X|)), 1-b) - p + 1
        const mpfr_float abs_x = abs(X);
        const long long e_unbounded = floor(log2(abs_x)).convert_to<long long>();
        const long long E = std::max<long long>(e_unbounded, 1 - Bias) - static_cast<long long>(P) + 1;

        // S = |X| * 2^(-E)
        const mpfr_float S = abs_x * pow(mpfr_float(2.0), -E);

        const long long floor_s = floor(S).convert_to<long long>();
        // eta = S - floor(S)
        const mpfr_float fraction = S - floor_s;

        // CodeIsEven definition from spec.
        const bool code_is_even = (P > 1)
                                      ? detail::is_even_i64(floor_s)
                                      : (floor_s == 0 || detail::is_even_i64(E + Bias));

        bool round_away = false;
        if constexpr (std::is_same_v<RM, TowardZero>)
        {
            round_away = false;
        }
        else if constexpr (std::is_same_v<RM, TowardMaxMagnitude>)
        {
            round_away = fraction > 0;
        }
        else if constexpr (std::is_same_v<RM, TowardPositive>)
        {
            round_away = (fraction > 0) && (X > 0);
        }
        else if constexpr (std::is_same_v<RM, TowardNegative>)
        {
            round_away = (fraction > 0) && (X < 0);
        }
        else if constexpr (std::is_same_v<RM, NearestTiesToAway>)
        {
            round_away = fraction >= mpfr_half;
        }
        else if constexpr (std::is_same_v<RM, NearestTiesToEven>)
        {
            round_away = (fraction > mpfr_half) || ((fraction == mpfr_half) && !code_is_even);
        }
        else if constexpr (std::is_same_v<RM, ToOdd>)
        {
            round_away = (fraction > 0) && code_is_even;
        }
        else if constexpr (std::is_base_of_v<RoundingMode, RM> && requires { RM::bits; } && std::is_same_v<RM, StochasticA<RM::bits>>)
        {
            const mpfr_float pow2n = pow(mpfr_float(2.0), static_cast<long long>(RM::bits));
            const mpfr_float lhs = floor(fraction * pow2n) + mode.random;
            round_away = lhs >= pow2n;
        }
        else if constexpr (std::is_base_of_v<RoundingMode, RM> && requires { RM::bits; } && std::is_same_v<RM, StochasticB<RM::bits>>)
        {
            const mpfr_float pow2n1 = pow(mpfr_float(2.0), static_cast<long long>(RM::bits + 1));
            const mpfr_float lhs = floor(fraction * pow2n1) + (2 * mode.random + 1);
            round_away = lhs >= pow2n1;
        }
        else if constexpr (std::is_base_of_v<RoundingMode, RM> && requires { RM::bits; } && std::is_same_v<RM, StochasticC<RM::bits>>)
        {
            const mpfr_float pow2n = pow(mpfr_float(2.0), static_cast<long long>(RM::bits));
            const long long rn = detail::rnite_ties_to_even(fraction * pow2n);
            round_away = (mpfr_float(rn) + mode.random) >= pow2n;
        }
        else
        {
            static_assert(std::is_base_of_v<RoundingMode, RM>, "Unsupported rounding mode policy type");
        }

        const long long I = floor_s + (round_away ? 1 : 0);
        const mpfr_float sign = (X < 0) ? mpfr_float(-1.0) : mpfr_float(1.0);
        return sign * mpfr_float(I) * pow(mpfr_float(2.0), E);
    }

} // namespace p3109
