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

} // namespace p3109

#include "ops/Decode.h"
#include "ops/Encode.h"
