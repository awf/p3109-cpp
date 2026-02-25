#include <cstdlib>
#include <iostream>
#include <string>

#include <boost/math/special_functions/fpclassify.hpp>

#include "p3109.h"
#include "test_utils.h"

namespace
{
    using Binary8p3 = p3109::binary<8, 3, p3109::Signed, p3109::Extended>;

    bool test_zero()
    {
        return test_utils::expect_equal(p3109::Decode(Binary8p3{0}), p3109::mpfr_float(0), "Decode(0) should be 0");
    }

    bool test_subnormal()
    {
        // x=1 => T=1, E=0 => 2^-17
        return test_utils::expect_equal(
            p3109::Decode(Binary8p3{1}),
            pow(p3109::mpfr_float(2), -17),
            "Decode(1) should be 2^-17");
    }

    bool test_normal()
    {
        // x=4 => T=0, E=1 => 2^-15
        return test_utils::expect_equal(
            p3109::Decode(Binary8p3{4}),
            pow(p3109::mpfr_float(2), -15),
            "Decode(4) should be 2^-15");
    }

    bool test_reflection()
    {
        const auto positive = p3109::Decode(Binary8p3{1});
        const auto reflected = p3109::Decode(Binary8p3{129});
        return test_utils::expect_equal(reflected, -positive, "Decode(129) should be -Decode(1)");
    }

    bool test_special_values()
    {
        bool ok = true;

        const auto plus_inf = p3109::Decode(Binary8p3{127});
        const auto nan_value = p3109::Decode(Binary8p3{128});
        const auto minus_inf = p3109::Decode(Binary8p3{255});

        ok &= test_utils::expect_true(boost::math::isinf(plus_inf) && plus_inf > 0, "Decode(127) should be +inf");
        ok &= test_utils::expect_true(boost::math::isnan(nan_value), "Decode(128) should be NaN");
        ok &= test_utils::expect_true(boost::math::isinf(minus_inf) && minus_inf < 0, "Decode(255) should be -inf");

        return ok;
    }

    int run_case(const std::string &test_name)
    {
        p3109::ensure_mpfr_precision();

        if (test_name == "zero")
            return test_zero() ? EXIT_SUCCESS : EXIT_FAILURE;
        if (test_name == "subnormal")
            return test_subnormal() ? EXIT_SUCCESS : EXIT_FAILURE;
        if (test_name == "normal")
            return test_normal() ? EXIT_SUCCESS : EXIT_FAILURE;
        if (test_name == "reflection")
            return test_reflection() ? EXIT_SUCCESS : EXIT_FAILURE;
        if (test_name == "special_values")
            return test_special_values() ? EXIT_SUCCESS : EXIT_FAILURE;

        std::cerr << "Unknown test case: " << test_name << '\n';
        return EXIT_FAILURE;
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: p3109_tests <test-name>\n";
        return EXIT_FAILURE;
    }

    return run_case(argv[1]);
}
