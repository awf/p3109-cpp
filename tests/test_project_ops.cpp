#include <array>
#include <stdexcept>
#include <string>

#include <boost/math/special_functions/fpclassify.hpp>

#include "p3109_test_runner.h"
#include "test_utils.h"

namespace
{
    template <unsigned K, unsigned P, p3109::Signedness Sigma, p3109::Domain Delta>
    struct TestProjectOps
    {
        using float_type = p3109::binary<K, P, Sigma, Delta>;
        using test_fn = test_utils::test_fn<TestProjectOps>;
        using test_case = test_utils::test_case<TestProjectOps>;

        static constexpr int bias = float_type::exponent_bias;

        bool test_round_ties_to_even()
        {
            const auto x = p3109::mpfr_float("1.625");
            const auto z = p3109::RoundToPrecision<P, bias, p3109::NearestTiesToEven>(x);
            return test_utils::expect_equal(z, p3109::mpfr_float("1.5"), "RoundToPrecision ties-to-even should round 1.625 to 1.5 at p=3");
        }

        bool test_round_ties_to_away()
        {
            const auto x = p3109::mpfr_float("1.625");
            const auto z = p3109::RoundToPrecision<P, bias, p3109::NearestTiesToAway>(x);
            return test_utils::expect_equal(z, p3109::mpfr_float("1.75"), "RoundToPrecision ties-to-away should round 1.625 to 1.75 at p=3");
        }

        bool test_saturate_finite()
        {
            const auto max_finite = p3109::Decode(float_type{p3109::pow2_u64(K - 1) - 2});
            const auto z = p3109::Saturate<Sigma, Delta, p3109::NearestTiesToEven>(p3109::mpfr_inf, max_finite, p3109::SaturationMode::SatFinite);
            return test_utils::expect_equal(z, max_finite, "SatFinite should clamp +inf to max finite");
        }

        bool test_saturate_propagate()
        {
            const auto max_finite = p3109::Decode(float_type{p3109::pow2_u64(K - 1) - 2});
            const auto z = p3109::Saturate<Sigma, Delta, p3109::NearestTiesToEven>(p3109::mpfr_inf, max_finite, p3109::SaturationMode::SatPropagate);
            return test_utils::expect_true(boost::math::isinf(z) && z > 0, "SatPropagate should preserve +inf");
        }

        bool test_saturate_ovf_inf_toward_zero()
        {
            const auto max_finite = p3109::Decode(float_type{p3109::pow2_u64(K - 1) - 2});
            const auto z = p3109::Saturate<Sigma, Delta, p3109::TowardZero>(max_finite * 2, max_finite, p3109::SaturationMode::OvfInf);
            return test_utils::expect_equal(z, max_finite, "OvfInf + TowardZero should clamp positive overflow to max finite");
        }

        bool test_saturate_ovf_inf_nearest()
        {
            const auto max_finite = p3109::Decode(float_type{p3109::pow2_u64(K - 1) - 2});
            const auto z = p3109::Saturate<Sigma, Delta, p3109::NearestTiesToEven>(max_finite * 2, max_finite, p3109::SaturationMode::OvfInf);
            return test_utils::expect_true(boost::math::isinf(z) && z > 0, "OvfInf + nearest should map positive overflow to +inf");
        }

        bool test_project_nan()
        {
            using Format = p3109::binary<K, P, Sigma, Delta>;
            const auto z = p3109::Project<Format, p3109::NearestTiesToEven>(p3109::mpfr_nan, p3109::SaturationMode::SatFinite);
            return test_utils::expect_true(z.codepoint == p3109::pow2_u64(K - 1), "Project(NaN) should return NaN codepoint");
        }

        bool test_project_ovf_inf_nearest()
        {
            using Format = p3109::binary<K, P, Sigma, Delta>;
            const auto z = p3109::Project<Format, p3109::NearestTiesToEven>(
                p3109::mpfr_float("1e100"),
                p3109::SaturationMode::OvfInf);
            return test_utils::expect_true(z.codepoint == (p3109::pow2_u64(K - 1) - 1), "Project OvfInf + nearest should map overflow to +inf codepoint");
        }

        bool test_project_ovf_inf_toward_zero()
        {
            using Format = p3109::binary<K, P, Sigma, Delta>;
            const auto z = p3109::Project<Format, p3109::TowardZero>(
                p3109::mpfr_float("1e100"),
                p3109::SaturationMode::OvfInf);
            return test_utils::expect_true(z.codepoint == (p3109::pow2_u64(K - 1) - 2), "Project OvfInf + TowardZero should clamp to max finite codepoint");
        }

        bool test_project_finite_requires_satfinite()
        {
            try
            {
                using Format = p3109::binary<K, P, Sigma, p3109::Finite>;
                (void)p3109::Project<Format, p3109::NearestTiesToEven>(
                    p3109::mpfr_float(1.0),
                    p3109::SaturationMode::SatPropagate);
                return test_utils::expect_true(false, "Finite-domain Project must reject non-SatFinite saturation mode");
            }
            catch (const std::invalid_argument &)
            {
                return true;
            }
        }

        static const std::array<test_case, 10> &all_test_cases()
        {
            static const std::array<test_case, 10> cases{{
                {"round_ties_to_even", &TestProjectOps::test_round_ties_to_even},
                {"round_ties_to_away", &TestProjectOps::test_round_ties_to_away},
                {"saturate_finite", &TestProjectOps::test_saturate_finite},
                {"saturate_propagate", &TestProjectOps::test_saturate_propagate},
                {"saturate_ovf_inf_toward_zero", &TestProjectOps::test_saturate_ovf_inf_toward_zero},
                {"saturate_ovf_inf_nearest", &TestProjectOps::test_saturate_ovf_inf_nearest},
                {"project_nan", &TestProjectOps::test_project_nan},
                {"project_ovf_inf_nearest", &TestProjectOps::test_project_ovf_inf_nearest},
                {"project_ovf_inf_toward_zero", &TestProjectOps::test_project_ovf_inf_toward_zero},
                {"project_finite_requires_satfinite", &TestProjectOps::test_project_finite_requires_satfinite},
            }};
            return cases;
        }
    };

    template <typename TestSuite>
    p3109::tests::run_status run_suite(const std::string &test_name)
    {
        p3109::ensure_mpfr_precision();

        TestSuite suite;
        for (const auto &test : TestSuite::all_test_cases())
        {
            if (test_name == test.name)
                return (suite.*(test.fn))() ? p3109::tests::run_status::pass : p3109::tests::run_status::fail;
        }
        return p3109::tests::run_status::unsupported;
    }
}

namespace p3109::tests
{
    run_status run_project_ops(unsigned k, unsigned p, Signedness sigma, const std::string &test_name)
    {
        if (k == 8 && p == 3 && sigma == Signed)
            return run_suite<TestProjectOps<8, 3, Signed, Extended>>(test_name);

        return run_status::unsupported;
    }
}
