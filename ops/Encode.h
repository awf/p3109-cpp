#pragma once

#include <algorithm>

#include <boost/math/special_functions/fpclassify.hpp>

#include "p3109.h"

namespace p3109
{
    template <unsigned K, unsigned P, Signedness Sigma = Signed, Domain Delta = Extended>
    std::uint64_t EncodeAux(mpfr_float X)
    {
        ensure_mpfr_precision();

        constexpr std::uint64_t two_to_k = pow2_u64(K);
        constexpr std::uint64_t two_to_km1 = pow2_u64(K - 1);

        if (boost::math::isnan(X))
            return (Sigma == Signed) ? two_to_km1 : (two_to_k - 1);

        if (boost::math::isinf(X))
        {
            if (X > 0)
                return (Sigma == Signed) ? (two_to_km1 - 1) : (two_to_k - 2);

            if constexpr (Sigma == Signed)
                return EncodeAux<K, P, Signed, Delta>(-X) + two_to_km1;

            return two_to_k - 1;
        }

        if constexpr (Sigma == Signed)
        {
            if (X < 0)
                return EncodeAux<K, P, Signed, Delta>(-X) + two_to_km1;
        }

        if (X == 0)
            return 0;

        constexpr std::uint64_t trailing_modulus = pow2_u64(P - 1);
        constexpr int bias = binary<K, P, Sigma, Delta>::exponent_bias;

        const auto e_floor = floor(log2(X)).convert_to<long>();
        const long E = std::max<long>(e_floor, 1 - bias);

        const mpfr_float S = X * pow(mpfr_float(2), static_cast<long>(P - 1) - E);
        const mpfr_float Tm = fmod(S, mpfr_float(trailing_modulus));
        const std::uint64_t T = Tm.convert_to<std::uint64_t>();

        if (S < mpfr_float(trailing_modulus))
            return T;

        const long stored_exp = E + bias;
        return T + static_cast<std::uint64_t>(stored_exp) * trailing_modulus;
    }

    template <unsigned K, unsigned P, Signedness Sigma = Signed, Domain Delta = Extended>
    binary<K, P, Sigma, Delta> Encode(mpfr_float x)
    {
        return binary<K, P, Sigma, Delta>{EncodeAux<K, P, Sigma, Delta>(x)};
    }
} // namespace p3109
