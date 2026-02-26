#pragma once

#include <stdexcept>
#include <type_traits>

#include "p3109.h"
#include "ops/Format.h"

namespace p3109
{
    // Project<Format, projspec>(X) -> x
    //
    // Structured definition alignment (main.tex §Project):
    // - Parameters:
    //     Format   = target format type (e.g., binary<K, P, Sigma, Delta>)
    //     projspec = projection specification (sat, round) or ProjectionSpec<RM>
    // - Operand:
    //     X = closed extended real value
    // - Result:
    //     x = P3109 value in format Format
    //
    // Behavior mapping used below:
    //   R = RoundToPrecision<Format::precision, Format::exponent_bias, RM>(X)
    //   S = Saturate<Format::signedness, Format::domain, RM>(R, MaxFinite(Format), sat, round)
    //   x = Encode<Format>(S)

    template <typename Format, typename RM = NearestTiesToEven>
    Format Project(mpfr_float X,
                   SaturationMode sat,
                   RM roundMode = RM{})
    {
        static_assert(std::is_base_of_v<RoundingMode, RM>, "RM must derive from RoundingMode");

        if constexpr (Format::domain == Finite)
        {
            if (sat != SaturationMode::SatFinite)
                throw std::invalid_argument("Finite-domain Project requires SatFinite");
        }

        if (boost::math::isnan(X))
            return Encode<Format>(X);

        constexpr int bias = Format::exponent_bias;
        const mpfr_float max_finite = Decode(MaxFiniteOf<Format>());
        const mpfr_float rounded = RoundToPrecision<Format::precision, bias, RM>(X, roundMode);
        const mpfr_float saturated = Saturate<Format::signedness, Format::domain, RM>(rounded, max_finite, sat, roundMode);
        return Encode<Format>(saturated);
    }

    template <typename Format, typename RM>
    Format Project(mpfr_float X, ProjectionSpec<RM> projectionSpec)
    {
        return Project<Format, RM>(X, projectionSpec.saturate, projectionSpec.round);
    }
} // namespace p3109
