#include <array>
#include <string>

#include "p3109_test_runner.h"
#include "test_utils.h"

namespace
{
    template <unsigned K, unsigned P, p3109::Signedness Sigma, p3109::Domain Delta>
    struct TestSqrt
    {
        using format_type = p3109::binary<K, P, Sigma, Delta>;
        using test_fn = test_utils::test_fn<TestSqrt>;
        using test_case = test_utils::test_case<TestSqrt>;

        bool test_nan()
        {
            const auto z = p3109::Sqrt<format_type, format_type>(format_type::nan);
            return test_utils::expect_true(z.isnan(), "Sqrt(NaN) should return NaN");
        }

        bool test_neginf()
        {
            const auto z = p3109::Sqrt<format_type, format_type>(format_type::ninf);
            return test_utils::expect_true(z.isnan(), "Sqrt(-inf) should return NaN");
        }

        bool test_negative()
        {
            const auto x = p3109::Encode<format_type>(p3109::mpfr_float(-1.0));
            const auto z = p3109::Sqrt<format_type, format_type>(x);
            return test_utils::expect_true(z.isnan(), "Sqrt(X) with X<0 should return NaN");
        }

        bool test_posinf()
        {
            const auto z = p3109::Sqrt<format_type, format_type>(format_type::inf);
            return test_utils::expect_true(z.isposinf(), "Sqrt(+inf) should return +inf");
        }

        bool test_positive()
        {
            const auto x = p3109::Encode<format_type>(p3109::mpfr_float(4.0));
            const auto z = p3109::Sqrt<format_type, format_type>(x);
            const auto expected = p3109::Encode<format_type>(p3109::mpfr_float(2.0));
            return test_utils::expect_true(z.codepoint == expected.codepoint, "Sqrt(4) should return 2");
        }

        static const std::array<test_case, 5> &all_test_cases()
        {
            static const std::array<test_case, 5> cases{{
                {"sqrt_nan", &TestSqrt::test_nan},
                {"sqrt_neginf", &TestSqrt::test_neginf},
                {"sqrt_negative", &TestSqrt::test_negative},
                {"sqrt_posinf", &TestSqrt::test_posinf},
                {"sqrt_positive", &TestSqrt::test_positive},
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
    run_status run_sqrt(unsigned k, unsigned p, Signedness sigma, const std::string &test_name)
    {
        if (k == 8 && p == 3 && sigma == Signed)
            return run_suite<TestSqrt<8, 3, Signed, Extended>>(test_name);

        return run_status::unsupported;
    }
}
