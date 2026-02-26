#pragma once

#include <cstdint>
#include "p3109.h"

namespace p3109
{
    // MaxFiniteOf<Format>: Returns the Format value with the maximum finite codepoint for a given format.
    // (Spec-aligned: see main.tex for full set of format-derived constants.)
    template <typename Format>
    constexpr Format MaxFiniteOf()
    {
        constexpr unsigned K = Format::bitwidth;
        constexpr Signedness Sigma = Format::signedness;
        constexpr Domain Delta = Format::domain;
        constexpr std::uint64_t two_to_k = pow2_u64(K);
        constexpr std::uint64_t two_to_km1 = pow2_u64(K - 1);

        std::uint64_t codepoint = 0;
        if constexpr (Sigma == Signed)
        {
            if constexpr (Delta == Extended)
                codepoint = two_to_km1 - 2;
            else
                codepoint = two_to_km1 - 1;
        }
        else
        {
            if constexpr (Delta == Extended)
                codepoint = two_to_k - 3;
            else
                codepoint = two_to_k - 2;
        }
        return Format{codepoint};
    }
    // TODO: Add MinPositiveOf, ExponentBiasOf, etc. as needed from main.tex
}
