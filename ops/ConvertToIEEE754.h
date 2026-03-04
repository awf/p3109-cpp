#pragma once

#include <limits>
#include <type_traits>

#include <boost/math/special_functions/fpclassify.hpp>

#include "ops/Decode.h"
#include "ops/RoundToPrecision.h"
#include "ops/Saturate.h"
#include "p3109.h"

namespace p3109 {
  namespace detail {
    // Traits for IEEE 754 native C++ types.
    // Provides precision, exponent bias, and max finite value
    // needed by ConvertToIEEE754.
    template <typename T>
    struct IEEETraits;

    template <>
    struct IEEETraits<float> {
      static constexpr unsigned precision = 24;
      static constexpr int exponent_bias = 127;
      static const mpfr_float &max_finite()
      {
        ensure_mpfr_precision();
        static const mpfr_float val(std::numeric_limits<float>::max());
        return val;
      }
    };

    template <>
    struct IEEETraits<double> {
      static constexpr unsigned precision = 53;
      static constexpr int exponent_bias = 1023;
      static const mpfr_float &max_finite()
      {
        ensure_mpfr_precision();
        static const mpfr_float val(std::numeric_limits<double>::max());
        return val;
      }
    };

    // Encode754<phi>(X) -> native IEEE 754 value
    //
    // Converts a closed extended real (already rounded and saturated)
    // to a native C++ IEEE 754 type.
    //
    // Spec (main.tex §Encode754):
    //   Encode754(NaN) -> any quiet IEEE NaN
    //   Encode754(X)   -> the code in phi that decodes to X
    template <typename IEEEType>
    IEEEType Encode754(const mpfr_float &X)
    {
      if (boost::math::isnan(X))
        return std::numeric_limits<IEEEType>::quiet_NaN();
      if (boost::math::isinf(X))
        return X > 0 ? std::numeric_limits<IEEEType>::infinity() : -std::numeric_limits<IEEEType>::infinity();
      return X.convert_to<IEEEType>();
    }
  } // namespace detail

  // ConvertToIEEE754<IEEEType>(x, projSpec) -> X
  //
  // Convert a P3109 value to an IEEE 754 format (main.tex §ConvertToIEEE754).
  //
  // Behavior:
  //   ConvertToIEEE754(NaN) -> Encode754(NaN)
  //   ConvertToIEEE754(x)   -> Encode754(Saturate(RoundToPrecision(Decode(x))))
  //
  // Where:
  //   Y = Decode<f>(x)
  //   R = RoundToPrecision<P_phi, B_phi, Round>(Y)
  //   X = Saturate<Signed, Extended, SM>(R, MAXFLOAT_phi)
  //
  // IEEE 754 is always Signed, Extended.
  template <typename IEEEType, typename FormatX, typename RM, typename SM>
  IEEEType ConvertToIEEE754(FormatX x, ProjectionSpec<RM, SM> projSpec)
  {
    using Traits = detail::IEEETraits<IEEEType>;

    if (x.isnan())
      return std::numeric_limits<IEEEType>::quiet_NaN();

    mpfr_float Y = Decode<FormatX>(x);
    mpfr_float R = RoundToPrecision<Traits::precision, Traits::exponent_bias, RM>(Y, projSpec.round);
    // IEEE 754 is always Signed, Extended
    mpfr_float X = Saturate<Signedness::Signed, Domain::Extended, SM, RM>(R, Traits::max_finite(), projSpec.round);
    return detail::Encode754<IEEEType>(X);
  }
} // namespace p3109
