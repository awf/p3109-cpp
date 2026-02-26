#include <cstdint>
#include <string>

#include <boost/math/special_functions/fpclassify.hpp>

#include "test_utils.h"

template <unsigned K, unsigned P, p3109::Signedness Sigma, p3109::Domain Delta>
struct TestDecode
{
    using float_type = p3109::binary<K, P, Sigma, Delta>;

    static constexpr std::uint64_t two_to_k = p3109::pow2_u64(K);
    static constexpr std::uint64_t two_to_km1 = p3109::pow2_u64(K - 1);
    static constexpr std::uint64_t trailing_modulus = p3109::pow2_u64(P - 1);

    static constexpr int bias = float_type::exponent_bias;

    static constexpr int subnormal_exp = 2 - static_cast<int>(P) - bias;
    static constexpr int normal_exp = 1 - bias;

    static bool test_zero()
    {
        return test_utils::expect_equal(p3109::Decode(float_type{0}), p3109::mpfr_float(0.0), "Decode(0) should be 0");
    }

    static bool test_subnormal()
    {
        return test_utils::expect_equal(
            p3109::Decode(float_type{1}),
            pow(p3109::mpfr_float(2.0), subnormal_exp),
            "Decode(1) should match subnormal formula");
    }

    static bool test_normal()
    {
        return test_utils::expect_equal(
            p3109::Decode(float_type{trailing_modulus}),
            pow(p3109::mpfr_float(2.0), normal_exp),
            "Decode(2^(P-1)) should match normal formula");
    }

    static bool test_reflection()
    {
        if constexpr (Sigma == p3109::Unsigned)
            return true;

        const std::uint64_t reflected_codepoint = two_to_km1 + 1;
        const auto positive = p3109::Decode(float_type{1});
        const auto reflected = p3109::Decode(float_type{reflected_codepoint});
        return test_utils::expect_equal(reflected, -positive, "Signed reflection should hold for codepoint 1");
    }

    static bool test_special_values()
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

    static void run()
    {
        using test_utils::run_test;
        run_test({float_type::name(), "zero"}, test_zero());
        run_test({float_type::name(), "subnormal"}, test_subnormal());
        run_test({float_type::name(), "normal"}, test_normal());
        run_test({float_type::name(), "reflection"}, test_reflection());
        run_test({float_type::name(), "special_values"}, test_special_values());
    }
};

int main()
{
    test_utils::init("decode");

    using Binary8p3se = p3109::binary<8, 3, p3109::Signed, p3109::Extended>;
    using Binary8p4se = p3109::binary<8, 4, p3109::Signed, p3109::Extended>;
    using Binary8p3ue = p3109::binary<8, 3, p3109::Unsigned, p3109::Extended>;
    using Binary8p4ue = p3109::binary<8, 4, p3109::Unsigned, p3109::Extended>;

    TestDecode<8, 3, p3109::Signed, p3109::Extended>::run();
    TestDecode<8, 4, p3109::Signed, p3109::Extended>::run();
    TestDecode<8, 3, p3109::Unsigned, p3109::Extended>::run();
    TestDecode<8, 4, p3109::Unsigned, p3109::Extended>::run();

    return test_utils::finalize();
}
