#include "test_utils.h"
#include "ops/Format.h"

template <typename Format>
struct TestFormat {
  static constexpr int bias = Format::exponent_bias;
  static constexpr int subnormal_exp = 2 - static_cast<int>(Format::precision) - bias;

  static bool test_min_positive_codepoint()
  {
    const auto min_pos = p3109::MinPositiveOf<Format>();
    return test_utils::expect_true(min_pos.codepoint == 1, "MinPositiveOf should have codepoint 1");
  }

  static bool test_min_positive_value()
  {
    const auto min_pos = p3109::MinPositiveOf<Format>();
    const auto decoded = p3109::Decode(min_pos);
    const auto expected = pow(p3109::mpfr_float(2.0), subnormal_exp);
    return test_utils::expect_equal(decoded, expected, "MinPositiveOf should decode to smallest positive value");
  }

  static bool test_min_finite()
  {
    const auto min_fin = p3109::Decode(p3109::MinFiniteOf<Format>());
    const auto max_fin = p3109::Decode(p3109::MaxFiniteOf<Format>());
    if constexpr (Format::is_signed)
      return test_utils::expect_equal(min_fin, -max_fin, "MinFiniteOf should be the negation of MaxFiniteOf for signed formats");
    else
      return test_utils::expect_equal(min_fin, p3109::mpfr_float(0), "MinFiniteOf should be zero for unsigned formats");
  }

  static bool test_min_normal_codepoint()
  {
    const auto min_norm = p3109::MinNormalOf<Format>();
    return test_utils::expect_true(
      min_norm.codepoint == p3109::pow2_u64(Format::precision - 1), "MinNormalOf should have codepoint 2^(P-1)");
  }

  static bool test_min_normal_greater_than_min_positive()
  {
    const auto min_norm = p3109::Decode(p3109::MinNormalOf<Format>());
    const auto min_pos = p3109::Decode(p3109::MinPositiveOf<Format>());
    return test_utils::expect_true(min_norm > min_pos, "MinNormalOf should be greater than MinPositiveOf");
  }

  static bool test_exponent_bits()
  {
    constexpr unsigned expected = Format::is_signed
      ? (Format::bitwidth - Format::precision - 1)
      : (Format::bitwidth - Format::precision);
    return test_utils::expect_true(
      p3109::ExponentBitsOf<Format>() == expected, "ExponentBitsOf should match K-P-1 (signed) or K-P (unsigned)");
  }

  static bool test_trailing_bits()
  {
    return test_utils::expect_true(
      p3109::TrailingBitsOf<Format>() == Format::precision - 1, "TrailingBitsOf should be P-1");
  }

  static bool test_exponent_bias()
  {
    return test_utils::expect_true(
      p3109::ExponentBiasOf<Format>() == Format::exponent_bias, "ExponentBiasOf should match format exponent_bias");
  }

  static bool test_max_finite_greater_than_min_positive()
  {
    const auto max_finite = p3109::Decode(p3109::MaxFiniteOf<Format>());
    const auto min_pos = p3109::Decode(p3109::MinPositiveOf<Format>());
    return test_utils::expect_true(max_finite > min_pos, "MaxFiniteOf should be greater than MinPositiveOf");
  }

  static void run(test_utils::suite &s)
  {
    s.with_path(Format::name(), [&s] {
      s.run({"min_positive_codepoint"}, test_min_positive_codepoint());
      s.run({"min_positive_value"}, test_min_positive_value());
      s.run({"min_finite"}, test_min_finite());
      s.run({"min_normal_codepoint"}, test_min_normal_codepoint());
      s.run({"min_normal_greater_than_min_positive"}, test_min_normal_greater_than_min_positive());
      s.run({"exponent_bits"}, test_exponent_bits());
      s.run({"trailing_bits"}, test_trailing_bits());
      s.run({"exponent_bias"}, test_exponent_bias());
      s.run({"max_finite_greater_than_min_positive"}, test_max_finite_greater_than_min_positive());
    });
  }
};

int main()
{
  test_utils::suite s{"format"};

  using SE83 = p3109::binary<8, 3, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using SE84 = p3109::binary<8, 4, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using UE83 = p3109::binary<8, 3, p3109::Signedness::Unsigned, p3109::Domain::Extended>;
  using UE84 = p3109::binary<8, 4, p3109::Signedness::Unsigned, p3109::Domain::Extended>;
  using SF83 = p3109::binary<8, 3, p3109::Signedness::Signed, p3109::Domain::Finite>;
  using UF83 = p3109::binary<8, 3, p3109::Signedness::Unsigned, p3109::Domain::Finite>;

  TestFormat<SE83>::run(s);
  TestFormat<SE84>::run(s);
  TestFormat<UE83>::run(s);
  TestFormat<UE84>::run(s);
  TestFormat<SF83>::run(s);
  TestFormat<UF83>::run(s);

  return s.finalize();
}

