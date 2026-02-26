#include <array>
#include <cstdint>
#include <string>

#include <boost/math/special_functions/fpclassify.hpp>

#include "p3109_test_runner.h"
#include "test_utils.h"

namespace
{
    template <unsigned K, unsigned P, p3109::Signedness Sigma, p3109::Domain Delta>
    struct TestEncode
    {
        using float_type = p3109::binary<K, P, Sigma, Delta>;
        using test_fn = test_utils::test_fn<TestEncode>;
        using test_case = test_utils::test_case<TestEncode>;

        static constexpr std::uint64_t two_to_k = p3109::pow2_u64(K);
        static constexpr std::uint64_t two_to_km1 = p3109::pow2_u64(K - 1);
        static constexpr std::uint64_t trailing_modulus = p3109::pow2_u64(P - 1);

        static constexpr int bias = float_type::exponent_bias;

        static constexpr int subnormal_exp = 2 - static_cast<int>(P) - bias;
        static constexpr int normal_exp = 1 - bias;

        bool test_zero()
        {
            using Format = p3109::binary<K, P, Sigma, Delta>;
            return test_utils::expect_true(p3109::Encode<Format>(p3109::mpfr_float(0.0)).codepoint == 0, "Encode(0) should be 0");
        }

        bool test_subnormal()
        {
            const auto x = pow(p3109::mpfr_float(2.0), subnormal_exp);
            return test_utils::expect_true(
                p3109::Encode<p3109::binary<K, P, Sigma, Delta>>(x).codepoint == 1,
                "Encode(min positive subnormal quantum) should be 1");
        }

        bool test_normal()
        {
            const auto x = pow(p3109::mpfr_float(2.0), normal_exp);
            return test_utils::expect_true(
                p3109::Encode<p3109::binary<K, P, Sigma, Delta>>(x).codepoint == trailing_modulus,
                "Encode(min normal) should be 2^(P-1)");
        }

        bool test_reflection()
        {
            if constexpr (Sigma == p3109::Unsigned)
                return true;

            const auto x = pow(p3109::mpfr_float(2.0), subnormal_exp);
            const auto pos = p3109::Encode<p3109::binary<K, P, Sigma, Delta>>(x).codepoint;
            const auto neg = p3109::Encode<p3109::binary<K, P, Sigma, Delta>>(-x).codepoint;
            return test_utils::expect_true(neg == pos + two_to_km1, "Signed reflection should hold for Encode");
        }

        bool test_special_values()
        {
            if constexpr (Sigma == p3109::Signed)
            {
                bool ok = true;
                ok &= test_utils::expect_true(p3109::Encode<p3109::binary<K, P, Sigma, Delta>>(p3109::mpfr_nan).codepoint == two_to_km1, "Signed NaN codepoint should be 2^(K-1)");
                ok &= test_utils::expect_true(p3109::Encode<p3109::binary<K, P, Sigma, Delta>>(p3109::mpfr_inf).codepoint == (two_to_km1 - 1), "Signed +inf codepoint should be 2^(K-1)-1");
                ok &= test_utils::expect_true(p3109::Encode<p3109::binary<K, P, Sigma, Delta>>(-p3109::mpfr_inf).codepoint == (two_to_k - 1), "Signed -inf codepoint should be 2^K-1");
                return ok;
            }

            bool ok = true;
            ok &= test_utils::expect_true(p3109::Encode<p3109::binary<K, P, Sigma, Delta>>(p3109::mpfr_nan).codepoint == (two_to_k - 1), "Unsigned NaN codepoint should be 2^K-1");
            ok &= test_utils::expect_true(p3109::Encode<p3109::binary<K, P, Sigma, Delta>>(p3109::mpfr_inf).codepoint == (two_to_k - 2), "Unsigned +inf codepoint should be 2^K-2");
            return ok;
        }

        bool test_roundtrip_sample()
        {
            for (std::uint64_t x : {0ull, 1ull, trailing_modulus, two_to_km1, two_to_k - 1})
            {
                using Format = p3109::binary<K, P, Sigma, Delta>;
                const auto y = p3109::Decode<Format>(float_type{x});
                const auto z = p3109::Encode<Format>(y).codepoint;
                if (!test_utils::expect_true(z == x, "Encode(Decode(x)) should equal x for sampled codepoints"))
                    return false;
            }
            return true;
        }

        static const std::array<test_case, 6> &all_test_cases()
        {
            static const std::array<test_case, 6> cases{{
                {"encode_zero", &TestEncode::test_zero},
                {"encode_subnormal", &TestEncode::test_subnormal},
                {"encode_normal", &TestEncode::test_normal},
                {"encode_reflection", &TestEncode::test_reflection},
                {"encode_special_values", &TestEncode::test_special_values},
                {"encode_roundtrip_sample", &TestEncode::test_roundtrip_sample},
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
            return run_suite<TestEncode<K, PMin, p3109::Signed, p3109::Extended>>(test_name);

        if constexpr (PMin < PMax)
            return run_for_p_signed<K, PMin + 1, PMax>(p, test_name);

        return p3109::tests::run_status::unsupported;
    }

    template <unsigned K, unsigned PMin, unsigned PMax>
    p3109::tests::run_status run_for_p_unsigned(unsigned p, const std::string &test_name)
    {
        if (p == PMin)
            return run_suite<TestEncode<K, PMin, p3109::Unsigned, p3109::Extended>>(test_name);

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
    run_status run_encode(unsigned k, unsigned p, Signedness sigma, const std::string &test_name)
    {
        constexpr unsigned KMin = 3;
        constexpr unsigned KMax = 16;

        if (k < KMin || k > KMax)
            return run_status::unsupported;

        return run_for_k<KMin, KMax>(k, p, test_name, sigma);
    }
}
