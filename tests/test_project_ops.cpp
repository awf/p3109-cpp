#include <stdexcept>

#include <boost/math/special_functions/fpclassify.hpp>

#include "test_utils.h"

template <unsigned K, unsigned P, p3109::Signedness Sigma, p3109::Domain Delta>
struct TestProjectOps {
  using float_type = p3109::binary<K, P, Sigma, Delta>;

  static constexpr int bias = float_type::exponent_bias;

  static bool test_round_ties_to_even()
  {
    const auto x = p3109::mpfr_float("1.625");
    const auto z = p3109::RoundToPrecision<P, bias, p3109::NearestTiesToEven>(x);
    return test_utils::expect_equal(
      z, p3109::mpfr_float("1.5"), "RoundToPrecision ties-to-even should round 1.625 to 1.5 at p=3");
  }

  static bool test_round_ties_to_away()
  {
    const auto x = p3109::mpfr_float("1.625");
    const auto z = p3109::RoundToPrecision<P, bias, p3109::NearestTiesToAway>(x);
    return test_utils::expect_equal(
      z, p3109::mpfr_float("1.75"), "RoundToPrecision ties-to-away should round 1.625 to 1.75 at p=3");
  }

  static bool test_saturate_finite()
  {
    const auto max_finite = p3109::Decode(float_type{p3109::pow2_u64(K - 1) - 2});
    const auto z = p3109::Saturate<Sigma, Delta, p3109::NearestTiesToEven>(
      p3109::mpfr_inf, max_finite, p3109::SaturationMode::SatFinite);
    return test_utils::expect_equal(z, max_finite, "SatFinite should clamp +inf to max finite");
  }

  static bool test_saturate_propagate()
  {
    const auto max_finite = p3109::Decode(float_type{p3109::pow2_u64(K - 1) - 2});
    const auto z = p3109::Saturate<Sigma, Delta, p3109::NearestTiesToEven>(
      p3109::mpfr_inf, max_finite, p3109::SaturationMode::SatPropagate);
    return test_utils::expect_true(boost::math::isinf(z) && z > 0, "SatPropagate should preserve +inf");
  }

  static bool test_saturate_ovf_inf_toward_zero()
  {
    const auto max_finite = p3109::Decode(float_type{p3109::pow2_u64(K - 1) - 2});
    const auto z =
      p3109::Saturate<Sigma, Delta, p3109::TowardZero>(max_finite * 2, max_finite, p3109::SaturationMode::OvfInf);
    return test_utils::expect_equal(z, max_finite, "OvfInf + TowardZero should clamp positive overflow to max finite");
  }

  static bool test_saturate_ovf_inf_nearest()
  {
    const auto max_finite = p3109::Decode(float_type{p3109::pow2_u64(K - 1) - 2});
    const auto z = p3109::Saturate<Sigma, Delta, p3109::NearestTiesToEven>(
      max_finite * 2, max_finite, p3109::SaturationMode::OvfInf);
    return test_utils::expect_true(
      boost::math::isinf(z) && z > 0, "OvfInf + nearest should map positive overflow to +inf");
  }

  static bool test_project_nan()
  {
    using Format = p3109::binary<K, P, Sigma, Delta>;
    const auto z = p3109::Project<Format, p3109::NearestTiesToEven>(p3109::mpfr_nan, p3109::SaturationMode::SatFinite);
    return test_utils::expect_true(z.codepoint == p3109::pow2_u64(K - 1), "Project(NaN) should return NaN codepoint");
  }

  static bool test_project_ovf_inf_nearest()
  {
    using Format = p3109::binary<K, P, Sigma, Delta>;
    const auto z =
      p3109::Project<Format, p3109::NearestTiesToEven>(p3109::mpfr_float("1e100"), p3109::SaturationMode::OvfInf);
    return test_utils::expect_true(
      z.codepoint == (p3109::pow2_u64(K - 1) - 1), "Project OvfInf + nearest should map overflow to +inf codepoint");
  }

  static bool test_project_ovf_inf_toward_zero()
  {
    using Format = p3109::binary<K, P, Sigma, Delta>;
    const auto z = p3109::Project<Format, p3109::TowardZero>(p3109::mpfr_float("1e100"), p3109::SaturationMode::OvfInf);
    return test_utils::expect_true(
      z.codepoint == (p3109::pow2_u64(K - 1) - 2), "Project OvfInf + TowardZero should clamp to max finite codepoint");
  }

  static bool test_project_finite_requires_satfinite()
  {
    try
    {
      using Format = p3109::binary<K, P, Sigma, p3109::Domain::Finite>;
      (void)p3109::Project<Format, p3109::NearestTiesToEven>(
        p3109::mpfr_float(1.0), p3109::SaturationMode::SatPropagate);
      return test_utils::expect_true(false, "Finite-domain Project must reject "
                                            "non-SatFinite saturation mode");
    }
    catch (const std::invalid_argument &)
    {
      return true;
    }
  }

  static void run(test_utils::suite &s)
  {
    s.with_path(float_type::name(), [&s] {
      s.run({"round_ties_to_even"}, test_round_ties_to_even());
      s.run({"round_ties_to_away"}, test_round_ties_to_away());
      s.run({"saturate_finite"}, test_saturate_finite());
      s.run({"saturate_propagate"}, test_saturate_propagate());
      s.run({"saturate_ovf_inf_toward_zero"}, test_saturate_ovf_inf_toward_zero());
      s.run({"saturate_ovf_inf_nearest"}, test_saturate_ovf_inf_nearest());
      s.run({"project_nan"}, test_project_nan());
      s.run({"project_ovf_inf_nearest"}, test_project_ovf_inf_nearest());
      s.run({"project_ovf_inf_toward_zero"}, test_project_ovf_inf_toward_zero());
      s.run({"project_finite_requires_satfinite"}, test_project_finite_requires_satfinite());
    });
  }
};

int main()
{
  test_utils::suite s{"project_ops"};

  TestProjectOps<8, 3, p3109::Signedness::Signed, p3109::Domain::Extended>::run(s);

  return s.finalize();
}
