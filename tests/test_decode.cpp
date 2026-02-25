#include <array>
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
        const std::uint64_t reflected_codepoint = two_to_km1 + 1;
        const auto positive = p3109::Decode(float_type{1});
        const auto reflected = p3109::Decode(float_type{reflected_codepoint});
        return test_utils::expect_equal(reflected, -positive, "Signed reflection should hold for codepoint 1");
    }

    bool test_special_values()
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
int run_suite(const std::string &test_name)
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

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: p3109_tests <test-name>\n";
        return EXIT_FAILURE;
    }
    auto test_name = std::string(argv[1]);

    bool ok = true;
    ok = ok && run_suite<TestDecode<8, 3, p3109::Signed, p3109::Extended>>(test_name);
    ok = ok && run_suite<TestDecode<8, 4, p3109::Signed, p3109::Extended>>(test_name);

    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
