#include <string>

#include "test_utils.h"

template <typename FormatX, typename FormatR, typename ProjSpec = p3109::ProjectionSpec<p3109::NearestTiesToEven>>
struct TestSqrt
{
    static bool test_nan()
    {
        const auto z = p3109::Sqrt<FormatX, FormatR, ProjSpec>(FormatX::nan);
        return test_utils::expect_true(z.isnan(), "Sqrt(NaN) should return NaN");
    }

    static bool test_neginf()
    {
        if constexpr (FormatX::signedness == p3109::Unsigned)
            return true;
        const auto z = p3109::Sqrt<FormatX, FormatR, ProjSpec>(FormatX::ninf);
        return test_utils::expect_true(z.isnan(), "Sqrt(-inf) should return NaN");
    }

    static bool test_negative()
    {
        if constexpr (FormatX::signedness == p3109::Unsigned)
            return true;
        const auto x = p3109::Encode<FormatX>(p3109::mpfr_float(-1.0));
        const auto z = p3109::Sqrt<FormatX, FormatR, ProjSpec>(x);
        return test_utils::expect_true(z.isnan(), "Sqrt(X) with X<0 should return NaN");
    }

    static bool test_posinf()
    {
        const auto z = p3109::Sqrt<FormatX, FormatR, ProjSpec>(FormatX::inf);
        return test_utils::expect_true(z.isposinf(), "Sqrt(+inf) should return +inf");
    }

    static bool test_positive()
    {
        const auto x = p3109::Encode<FormatX>(p3109::mpfr_float(4.0));
        const auto z = p3109::Sqrt<FormatX, FormatR, ProjSpec>(x);
        const auto expected = p3109::Encode<FormatR>(p3109::mpfr_float(2.0));
        return test_utils::expect_true(z.codepoint == expected.codepoint, "Sqrt(4) should return 2");
    }

    static void run()
    {
        using test_utils::run_test;
        run_test({FormatX::name(), FormatR::name(), "nan"}, test_nan());
        run_test({FormatX::name(), FormatR::name(), "neginf"}, test_neginf());
        run_test({FormatX::name(), FormatR::name(), "negative"}, test_negative());
        run_test({FormatX::name(), FormatR::name(), "posinf"}, test_posinf());
        run_test({FormatX::name(), FormatR::name(), "positive"}, test_positive());
    }
};

int main()
{
    test_utils::init("sqrt");

    using Binary8p3se = p3109::binary<8, 3, p3109::Signed, p3109::Extended>;
    using Binary8p4se = p3109::binary<8, 4, p3109::Signed, p3109::Extended>;
    using Binary8p3sf = p3109::binary<8, 3, p3109::Signed, p3109::Finite>;
    using Binary8p4sf = p3109::binary<8, 4, p3109::Signed, p3109::Finite>;
    using Binary8p3uf = p3109::binary<8, 3, p3109::Unsigned, p3109::Finite>;
    using Binary8p4uf = p3109::binary<8, 4, p3109::Unsigned, p3109::Finite>;

    TestSqrt<Binary8p3se, Binary8p3se>::run();
    TestSqrt<Binary8p4se, Binary8p4se>::run();
    TestSqrt<Binary8p3se, Binary8p4se>::run();
    TestSqrt<Binary8p4se, Binary8p3se>::run();

    TestSqrt<Binary8p3sf, Binary8p3sf>::run();
    TestSqrt<Binary8p4sf, Binary8p4sf>::run();
    TestSqrt<Binary8p3sf, Binary8p4sf>::run();
    TestSqrt<Binary8p4sf, Binary8p3sf>::run();

    TestSqrt<Binary8p3uf, Binary8p3uf>::run();
    TestSqrt<Binary8p4uf, Binary8p4uf>::run();
    TestSqrt<Binary8p3uf, Binary8p4uf>::run();
    TestSqrt<Binary8p4uf, Binary8p3uf>::run();

    return test_utils::finalize();
}
