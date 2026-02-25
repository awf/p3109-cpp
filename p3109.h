#pragma once

#include <cstdint>
#include <limits>
#include <boost/multiprecision/mpfr.hpp>

namespace p3109
{
    constexpr std::uint64_t pow2_u64(unsigned n)
    {
        return std::uint64_t{1} << n;
    }
}

namespace p3109
{
    using mpfr_float = boost::multiprecision::mpfr_float;
    const mpfr_float mpfr_nan = std::numeric_limits<mpfr_float>::quiet_NaN();
    const mpfr_float mpfr_inf = std::numeric_limits<mpfr_float>::infinity();

    enum Signedness
    {
        Signed,
        Unsigned
    };

    enum Domain
    {
        Finite,
        Extended
    };

    template <unsigned K, unsigned P, Signedness Sigma, Domain Delta>
    struct binary
    {
        static_assert(3 <= K, "K must be at least 3");
        static_assert(1 <= P, "P must be at least 1");
        static_assert(P <= (Sigma == Signed ? (K - 1) : K), "P must be < K for signed formats, and <= K for unsigned formats");
        static_assert(K < 63, "K must be < 63 for this uint64_t reference implementation");

        std::uint64_t codepoint;

        static const unsigned int exponent_bias = (Sigma == Signed)
                                                      ? pow2_u64(K - P - 1)
                                                      : pow2_u64(K - P);
    };

    inline void ensure_mpfr_precision()
    {
        static const bool configured = []()
        {
            constexpr unsigned kDecodePrecisionBits = 256;
            mpfr_float::default_precision(kDecodePrecisionBits);
            return true;
        }();
        (void)configured;
    }

    template <unsigned K, unsigned P, Signedness Sigma = Signed, Domain Delta = Extended>
    mpfr_float DecodeAux(std::uint64_t x)
    {
        ensure_mpfr_precision();

        constexpr std::uint64_t two_to_k = pow2_u64(K);
        constexpr std::uint64_t two_to_km1 = pow2_u64(K - 1);

        // NaN codepoints
        if (Sigma == Signed)
        {
            if (x == two_to_km1)
                return mpfr_nan;
        }
        else
        {
            if (x == (two_to_k - 1))
                return mpfr_nan;
        }

        // Infinity codepoints (Extended domain only)
        if (Delta == Extended)
        {
            if (Sigma == Signed)
            {
                if (x == (two_to_km1 - 1))
                    return mpfr_inf;
                if (x == (two_to_k - 1))
                    return -mpfr_inf;
            }
            else
            {
                if (x == (two_to_k - 2))
                    return mpfr_inf;
            }
        }

        // Signed reflection rule:
        // DecodeAux(Signed, domain, x) = -DecodeAux(Signed, domain, x - 2^(K-1)) for 2^(K-1) < x < 2^K
        if (Sigma == Signed)
        {
            if (x > two_to_km1 && x < two_to_k)
            {
                return -DecodeAux<K, P, Signed, Delta>(x - two_to_km1);
            }
        }

        // Common normal/subnormal decode
        constexpr std::uint64_t trailing_modulus = pow2_u64(P - 1);
        constexpr int bias = binary<K, P, Sigma, Delta>::exponent_bias;

        const std::uint64_t T = x % trailing_modulus;
        const std::uint64_t E = x / trailing_modulus;

        if (E == 0)
        {
            // Subnormals
            // (0 + T * 2^(1-P)) * 2^(1-bias)
            mpfr_float out = T;
            out *= pow(mpfr_float(2), 2 - static_cast<int>(P) - bias);
            return out;
        }

        // (1 + T * 2^(1-P)) * 2^(E-bias)
        mpfr_float t_scaled = T;
        t_scaled *= pow(mpfr_float(2), 1 - static_cast<int>(P));

        mpfr_float significand = 1;
        significand += t_scaled;
        significand *= pow(mpfr_float(2), static_cast<long>(E) - bias);
        return significand;
    }

    template <unsigned K, unsigned P, Signedness Sigma = Signed, Domain Delta = Extended>
    mpfr_float Decode(binary<K, P, Sigma, Delta> x)
    {
        return DecodeAux<K, P, Sigma, Delta>(x.codepoint);
    }

} // namespace p3109
