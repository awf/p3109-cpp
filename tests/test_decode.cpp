#include <array>
#include <cstdint>
#include <string>

#include <boost/math/special_functions/fpclassify.hpp>

#include "p3109_test_runner.h"
#include "test_utils.h"

namespace
{
    template <unsigned K, unsigned P, p3109::Signedness Sigma, p3109::Domain Delta>
    struct TestDecode
    {
        using float_type = p3109::binary<K, P, Sigma, Delta>;
        using test_fn = test_utils::test_fn<TestDecode>;
        using test_case = test_utils::test_case<TestDecode>;

        static constexpr std::uint64_t two_to_k = p3109::pow2_u64(K);
        static constexpr std::uint64_t two_to_km1 = p3109::pow2_u64(K - 1);
        static constexpr std::uint64_t trailing_modulus = p3109::pow2_u64(P - 1);

        static constexpr int bias = float_type::exponent_bias;

        static constexpr int subnormal_exp = 2 - static_cast<int>(P) - bias;
        static constexpr int normal_exp = 1 - bias;

        bool test_zero()
        {
            return test_utils::expect_equal(p3109::Decode(float_type{0}), p3109::mpfr_float(0.0), "Decode(0) should be 0");
        }

        bool test_subnormal()
        {
            return test_utils::expect_equal(
                p3109::Decode(float_type{1}),
                pow(p3109::mpfr_float(2.0), subnormal_exp),
                "Decode(1) should match subnormal formula");
        }

        bool test_normal()
        {
            return test_utils::expect_equal(
                p3109::Decode(float_type{trailing_modulus}),
                pow(p3109::mpfr_float(2.0), normal_exp),
                "Decode(2^(P-1)) should match normal formula");
        }

        bool test_reflection()
        {
            if constexpr (Sigma == p3109::Unsigned)
                return true;

            const std::uint64_t reflected_codepoint = two_to_km1 + 1;
            const auto positive = p3109::Decode(float_type{1});
            const auto reflected = p3109::Decode(float_type{reflected_codepoint});
            return test_utils::expect_equal(reflected, -positive, "Signed reflection should hold for codepoint 1");
        }

        bool test_special_values()
        {
            if constexpr (Sigma == p3109::Signed)
            {
                bool ok = true;

                const std::uint64_t plus_inf_codepoint = two_to_km1 - 1;
                const std::uint64_t nan_codepoint = two_to_km1;
                const std::uint64_t minus_inf_codepoint = two_to_k - 1;

                const auto plus_inf = p3109::Decode(float_type{plus_inf_codepoint});
                const auto nan_value = p3109::Decode(float_type{nan_codepoint});
                const auto minus_inf = p3109::Decode(float_type{minus_inf_codepoint});

                ok &= test_utils::expect_true(boost::math::isinf(plus_inf) && plus_inf > 0, "Signed +inf codepoint should decode to +inf");
                ok &= test_utils::expect_true(boost::math::isnan(nan_value), "Signed NaN codepoint should decode to NaN");
                ok &= test_utils::expect_true(boost::math::isinf(minus_inf) && minus_inf < 0, "Signed -inf codepoint should decode to -inf");
                return ok;
            }

            bool ok = true;

            const std::uint64_t plus_inf_codepoint = two_to_k - 2;
            const std::uint64_t nan_codepoint = two_to_k - 1;

            const auto plus_inf = p3109::Decode(float_type{plus_inf_codepoint});
            const auto nan_value = p3109::Decode(float_type{nan_codepoint});

            ok &= test_utils::expect_true(boost::math::isinf(plus_inf) && plus_inf > 0, "Unsigned +inf codepoint should decode to +inf");
            ok &= test_utils::expect_true(boost::math::isnan(nan_value), "Unsigned NaN codepoint should decode to NaN");
            return ok;
        }

        static const std::array<test_case, 5> &all_test_cases()
        {
            static const std::array<test_case, 5> cases{{
                {"decode_zero", &TestDecode::test_zero},
                {"decode_subnormal", &TestDecode::test_subnormal},
                {"decode_normal", &TestDecode::test_normal},
                {"decode_reflection", &TestDecode::test_reflection},
                {"decode_special_values", &TestDecode::test_special_values},
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

    template <unsigned K, unsigned PMin, unsigned PMax>
    p3109::tests::run_status run_for_p_signed(unsigned p, const std::string &test_name)
    {
        if (p == PMin)
            return run_suite<TestDecode<K, PMin, p3109::Signed, p3109::Extended>>(test_name);

        if constexpr (PMin < PMax)
            return run_for_p_signed<K, PMin + 1, PMax>(p, test_name);

        return p3109::tests::run_status::unsupported;
    }

    template <unsigned K, unsigned PMin, unsigned PMax>
    p3109::tests::run_status run_for_p_unsigned(unsigned p, const std::string &test_name)
    {
        if (p == PMin)
            return run_suite<TestDecode<K, PMin, p3109::Unsigned, p3109::Extended>>(test_name);

        if constexpr (PMin < PMax)
            return run_for_p_unsigned<K, PMin + 1, PMax>(p, test_name);

        return p3109::tests::run_status::unsupported;
    }

    template <unsigned KMin, unsigned KMax>
    p3109::tests::run_status run_for_k(unsigned k, unsigned p, const std::string &test_name, p3109::Signedness sigma)
    {
        if (k == KMin)
        {
            constexpr unsigned p_max_signed = KMin - 1;
            constexpr unsigned p_max_unsigned = KMin;
            const unsigned p_max = (sigma == p3109::Signed) ? p_max_signed : p_max_unsigned;

            if (p < 1 || p > p_max)
                return p3109::tests::run_status::unsupported;

            if (sigma == p3109::Signed)
                return run_for_p_signed<KMin, 1, p_max_signed>(p, test_name);
            return run_for_p_unsigned<KMin, 1, p_max_unsigned>(p, test_name);
        }

        if constexpr (KMin < KMax)
            return run_for_k<KMin + 1, KMax>(k, p, test_name, sigma);

        return p3109::tests::run_status::unsupported;
    }
}

namespace p3109::tests
{
    run_status run_decode(unsigned k, unsigned p, Signedness sigma, const std::string &test_name)
    {
        constexpr unsigned KMin = 3;
        constexpr unsigned KMax = 16;

        if (k < KMin || k > KMax)
            return run_status::unsupported;

        return run_for_k<KMin, KMax>(k, p, test_name, sigma);
    }
}
