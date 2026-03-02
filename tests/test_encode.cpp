#include <cstdint>

#include "test_utils.h"

template <unsigned K, unsigned P, p3109::Signedness Sigma, p3109::Domain Delta>
struct TestEncode {
  using float_type = p3109::binary<K, P, Sigma, Delta>;

  static constexpr std::uint64_t two_to_k = p3109::pow2_u64(K);
  static constexpr std::uint64_t two_to_km1 = p3109::pow2_u64(K - 1);
  static constexpr std::uint64_t trailing_modulus = p3109::pow2_u64(P - 1);

  static constexpr int bias = float_type::exponent_bias;

  static constexpr int subnormal_exp = 2 - static_cast<int>(P) - bias;
  static constexpr int normal_exp = 1 - bias;

  static bool test_zero()
  {
    using Format = p3109::binary<K, P, Sigma, Delta>;
    return test_utils::expect_true(
      p3109::Encode<Format>(p3109::mpfr_float(0.0)).codepoint == 0, "Encode(0) should be 0");
  }

  static bool test_subnormal()
  {
    const auto x = pow(p3109::mpfr_float(2.0), subnormal_exp);
    return test_utils::expect_true(p3109::Encode<p3109::binary<K, P, Sigma, Delta>>(x).codepoint == 1,
      "Encode(min positive subnormal quantum) should be 1");
  }

  static bool test_normal()
  {
    const auto x = pow(p3109::mpfr_float(2.0), normal_exp);
    return test_utils::expect_true(p3109::Encode<p3109::binary<K, P, Sigma, Delta>>(x).codepoint == trailing_modulus,
      "Encode(min normal) should be 2^(P-1)");
  }

  static bool test_reflection()
  {
    if constexpr (Sigma == p3109::Signedness::Unsigned)
      return true;

    const auto x = pow(p3109::mpfr_float(2.0), subnormal_exp);
    const auto pos = p3109::Encode<p3109::binary<K, P, Sigma, Delta>>(x).codepoint;
    const auto neg = p3109::Encode<p3109::binary<K, P, Sigma, Delta>>(-x).codepoint;
    return test_utils::expect_true(neg == pos + two_to_km1, "Signed reflection should hold for Encode");
  }

  static bool test_special_values()
  {
    if constexpr (Sigma == p3109::Signedness::Signed)
    {
      bool ok = true;
      ok &= test_utils::expect_true(
        p3109::Encode<p3109::binary<K, P, Sigma, Delta>>(p3109::mpfr_nan()).codepoint == two_to_km1,
        "Signed NaN codepoint should be 2^(K-1)");
      ok &= test_utils::expect_true(
        p3109::Encode<p3109::binary<K, P, Sigma, Delta>>(p3109::mpfr_inf()).codepoint == (two_to_km1 - 1),
        "Signed +inf codepoint should be 2^(K-1)-1");
      ok &= test_utils::expect_true(
        p3109::Encode<p3109::binary<K, P, Sigma, Delta>>(-p3109::mpfr_inf()).codepoint == (two_to_k - 1),
        "Signed -inf codepoint should be 2^K-1");
      return ok;
    }

    bool ok = true;
    ok &= test_utils::expect_true(
      p3109::Encode<p3109::binary<K, P, Sigma, Delta>>(p3109::mpfr_nan()).codepoint == (two_to_k - 1),
      "Unsigned NaN codepoint should be 2^K-1");
    ok &= test_utils::expect_true(
      p3109::Encode<p3109::binary<K, P, Sigma, Delta>>(p3109::mpfr_inf()).codepoint == (two_to_k - 2),
      "Unsigned +inf codepoint should be 2^K-2");
    return ok;
  }

  static bool test_roundtrip_sample()
  {
    for (std::uint64_t x :
      {std::uint64_t{0}, std::uint64_t{1}, trailing_modulus, two_to_km1, two_to_k - std::uint64_t{1}})
    {
      using Format = p3109::binary<K, P, Sigma, Delta>;
      const auto y = p3109::Decode<Format>(float_type{x});
      const auto z = p3109::Encode<Format>(y).codepoint;
      if (!test_utils::expect_true(z == x, "Encode(Decode(x)) should equal x for sampled codepoints"))
        return false;
    }
    return true;
  }

  static void run(test_utils::suite &s)
  {
    s.with_path(float_type::name(), [&s] {
      s.run({"zero"}, test_zero());
      s.run({"subnormal"}, test_subnormal());
      s.run({"normal"}, test_normal());
      s.run({"reflection"}, test_reflection());
      s.run({"special_values"}, test_special_values());
      s.run({"roundtrip_sample"}, test_roundtrip_sample());
    });
  }
};

int main()
{
  test_utils::suite s{"encode"};

  TestEncode<8, 3, p3109::Signedness::Signed, p3109::Domain::Extended>::run(s);
  TestEncode<8, 4, p3109::Signedness::Signed, p3109::Domain::Extended>::run(s);
  TestEncode<8, 3, p3109::Signedness::Unsigned, p3109::Domain::Extended>::run(s);
  TestEncode<8, 4, p3109::Signedness::Unsigned, p3109::Domain::Extended>::run(s);

  return s.finalize();
}
