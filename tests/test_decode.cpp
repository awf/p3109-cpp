#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

#include <boost/math/special_functions/fpclassify.hpp>

#include "p3109.h"
#include "test_utils.h"

template <unsigned K, unsigned P, p3109::Signedness Sigma, p3109::Domain Delta>
struct TestDecode
{
    using float_type = p3109::binary<K, P, Sigma, Delta>;
    using test_fn = bool (TestDecode::*)();

    struct test_case
    {
        const char *name;
        test_fn fn;
    };

    static constexpr std::uint64_t two_to_k = p3109::pow2_u64(K);
    static constexpr std::uint64_t two_to_km1 = p3109::pow2_u64(K - 1);
    static constexpr std::uint64_t trailing_modulus = p3109::pow2_u64(P - 1);

    static constexpr int bias = float_type::exponent_bias;

    static constexpr int subnormal_exp = 2 - static_cast<int>(P) - bias;
    static constexpr int normal_exp = 1 - bias;

    bool test_zero()
    {
        return test_utils::expect_equal(p3109::Decode(float_type{0}), p3109::mpfr_float(0), "Decode(0) should be 0");
    }

    bool test_subnormal()
    {
        // x=1 => T=1, E=0 => 2^(2-P-bias)
        return test_utils::expect_equal(
            p3109::Decode(float_type{1}),
            pow(p3109::mpfr_float(2), subnormal_exp),
            "Decode(1) should match subnormal formula");
    }

    bool test_normal()
    {
        // x=2^(P-1) => T=0, E=1 => 2^(1-bias)
        return test_utils::expect_equal(
            p3109::Decode(float_type{trailing_modulus}),
            pow(p3109::mpfr_float(2), normal_exp),
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
        else
        {
            bool ok = true;

            const std::uint64_t plus_inf_codepoint = two_to_k - 2;
            const std::uint64_t nan_codepoint = two_to_k - 1;

            const auto plus_inf = p3109::Decode(float_type{plus_inf_codepoint});
            const auto nan_value = p3109::Decode(float_type{nan_codepoint});

            ok &= test_utils::expect_true(boost::math::isinf(plus_inf) && plus_inf > 0, "Unsigned +inf codepoint should decode to +inf");
            ok &= test_utils::expect_true(boost::math::isnan(nan_value), "Unsigned NaN codepoint should decode to NaN");
            return ok;
        }
    }

    static const std::array<test_case, 5> &all_test_cases()
    {
        static const std::array<test_case, 5> cases{{
            {"zero", &TestDecode::test_zero},
            {"subnormal", &TestDecode::test_subnormal},
            {"normal", &TestDecode::test_normal},
            {"reflection", &TestDecode::test_reflection},
            {"special_values", &TestDecode::test_special_values},
        }};
        return cases;
    }
};

template <typename TestSuite>
bool run_suite(const std::string &test_name)
{
    p3109::ensure_mpfr_precision();

    TestSuite suite;

    for (const auto &test : TestSuite::all_test_cases())
    {
        if (test_name == test.name)
            return (suite.*(test.fn))();
    }

    std::cerr << "Unknown test case: " << test_name << '\n';
    return false;
}

template <unsigned K, unsigned PMin, unsigned PMax>
bool run_for_p_signed(unsigned p, const std::string &test_name)
{
    if (p == PMin)
        return run_suite<TestDecode<K, PMin, p3109::Signed, p3109::Extended>>(test_name);

    if constexpr (PMin < PMax)
        return run_for_p_signed<K, PMin + 1, PMax>(p, test_name);

    return false;
}

template <unsigned K, unsigned PMin, unsigned PMax>
bool run_for_p_unsigned(unsigned p, const std::string &test_name)
{
    if (p == PMin)
        return run_suite<TestDecode<K, PMin, p3109::Unsigned, p3109::Extended>>(test_name);

    if constexpr (PMin < PMax)
        return run_for_p_unsigned<K, PMin + 1, PMax>(p, test_name);

    return false;
}

template <unsigned KMin, unsigned KMax>
bool run_for_k(unsigned k, unsigned p, const std::string &test_name, p3109::Signedness sigma)
{
    if (k == KMin)
    {
        constexpr unsigned p_max_signed = KMin - 1;
        // NOTE: current DecodeAux implementation instantiates signed recursion,
        // so keep unsigned dispatch to P <= K-1 in this runner.
        constexpr unsigned p_max_unsigned = KMin - 1;
        const unsigned p_max = (sigma == p3109::Signed) ? p_max_signed : p_max_unsigned;

        if (p < 1 || p > p_max)
            return false;

        if (sigma == p3109::Signed)
            return run_for_p_signed<KMin, 1, p_max_signed>(p, test_name);
        return run_for_p_unsigned<KMin, 1, p_max_unsigned>(p, test_name);
    }

    if constexpr (KMin < KMax)
        return run_for_k<KMin + 1, KMax>(k, p, test_name, sigma);

    return false;
}

bool parse_signedness(const std::string &s, p3109::Signedness &out)
{
    if (s == "signed" || s == "Signed")
    {
        out = p3109::Signed;
        return true;
    }
    if (s == "unsigned" || s == "Unsigned")
    {
        out = p3109::Unsigned;
        return true;
    }
    return false;
}

bool parse_uint_arg(const char *arg, unsigned &out)
{
    try
    {
        const auto value = std::stoul(arg);
        out = static_cast<unsigned>(value);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        std::cerr << "Usage: p3109_tests <test-name> <K> <P> <signed|unsigned>\n";
        return EXIT_FAILURE;
    }

    const std::string test_name = argv[1];
    unsigned k = 0;
    unsigned p = 0;
    p3109::Signedness sigma = p3109::Signed;

    if (!parse_uint_arg(argv[2], k) || !parse_uint_arg(argv[3], p) || !parse_signedness(argv[4], sigma))
    {
        std::cerr << "Invalid arguments. Usage: p3109_tests <test-name> <K> <P> <signed|unsigned>\n";
        return EXIT_FAILURE;
    }

    if (k < 3 || k > 16)
    {
        std::cerr << "Unsupported K=" << k << ". Supported range is 3..16.\n";
        return EXIT_FAILURE;
    }

    const bool ok = run_for_k<3, 16>(k, p, test_name, sigma);
    if (!ok)
    {
        std::cerr << "Unsupported combination for K=" << k << ", P=" << p
                  << ", signedness=" << (sigma == p3109::Signed ? "signed" : "unsigned")
                  << ". Supported in this runner: 3<=K<=16 and 1<=P<=K-1.\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
