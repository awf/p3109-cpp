#include "test_utils.h"
#include "ops/Format.h"

template <unsigned K, unsigned P, p3109::Signedness Sigma, p3109::Domain Delta>
struct TestFormat {
  using float_type = p3109::binary<K, P, Sigma, Delta>;

  static constexpr int bias = float_type::exponent_bias;
  static constexpr int subnormal_exp = 2 - static_cast<int>(P) - bias;

  static bool test_min_positive_codepoint()
  {
    const auto min_pos = p3109::MinPositiveOf<float_type>();
    return test_utils::expect_true(min_pos.codepoint == 1, "MinPositiveOf should have codepoint 1");
  }

  static bool test_min_positive_value()
  {
    const auto min_pos = p3109::MinPositiveOf<float_type>();
    const auto decoded = p3109::Decode(min_pos);
    const auto expected = pow(p3109::mpfr_float(2.0), subnormal_exp);
    return test_utils::expect_equal(decoded, expected, "MinPositiveOf should decode to smallest positive value");
  }

  static bool test_exponent_bias()
  {
    constexpr unsigned expected_bias = float_type::exponent_bias;
    return test_utils::expect_true(
      p3109::ExponentBiasOf<float_type>() == expected_bias, "ExponentBiasOf should match format exponent_bias");
  }

  static bool test_max_finite_greater_than_min_positive()
  {
    const auto max_finite = p3109::Decode(p3109::MaxFiniteOf<float_type>());
    const auto min_pos = p3109::Decode(p3109::MinPositiveOf<float_type>());
    return test_utils::expect_true(max_finite > min_pos, "MaxFiniteOf should be greater than MinPositiveOf");
  }

  static void run(test_utils::suite &s)
  {
    s.with_path(float_type::name(), [&s] {
      s.run({"min_positive_codepoint"}, test_min_positive_codepoint());
      s.run({"min_positive_value"}, test_min_positive_value());
      s.run({"exponent_bias"}, test_exponent_bias());
      s.run({"max_finite_greater_than_min_positive"}, test_max_finite_greater_than_min_positive());
    });
  }
};

int main()
{
  test_utils::suite s{"format"};

  TestFormat<8, 3, p3109::Signedness::Signed, p3109::Domain::Extended>::run(s);
  TestFormat<8, 4, p3109::Signedness::Signed, p3109::Domain::Extended>::run(s);
  TestFormat<8, 3, p3109::Signedness::Unsigned, p3109::Domain::Extended>::run(s);
  TestFormat<8, 4, p3109::Signedness::Unsigned, p3109::Domain::Extended>::run(s);
  TestFormat<8, 3, p3109::Signedness::Signed, p3109::Domain::Finite>::run(s);
  TestFormat<8, 3, p3109::Signedness::Unsigned, p3109::Domain::Finite>::run(s);

  return s.finalize();
}
