#include <string>

#include "test_utils.h"

// CompareEqual tests: comparison operation exemplar
template <typename Format>
struct TestCompareEqual {
  static bool test_nan_lhs()
  {
    const auto one = p3109::Encode<Format>(p3109::mpfr_float(1.0));
    return test_utils::expect_true(!p3109::CompareEqual(Format::nan, one), "CompareEqual(NaN, 1) should be false");
  }

  static bool test_nan_rhs()
  {
    const auto one = p3109::Encode<Format>(p3109::mpfr_float(1.0));
    return test_utils::expect_true(!p3109::CompareEqual(one, Format::nan), "CompareEqual(1, NaN) should be false");
  }

  static bool test_nan_both()
  {
    return test_utils::expect_true(
      !p3109::CompareEqual(Format::nan, Format::nan), "CompareEqual(NaN, NaN) should be false");
  }

  static bool test_posinf_equal()
  {
    return test_utils::expect_true(
      p3109::CompareEqual(Format::inf, Format::inf), "CompareEqual(+inf, +inf) should be true");
  }

  static bool test_neginf_equal()
  {
    if constexpr (Format::signedness == p3109::Signedness::Unsigned)
      return true;
    return test_utils::expect_true(
      p3109::CompareEqual(Format::ninf, Format::ninf), "CompareEqual(-inf, -inf) should be true");
  }

  static bool test_posinf_vs_finite()
  {
    const auto one = p3109::Encode<Format>(p3109::mpfr_float(1.0));
    return test_utils::expect_true(!p3109::CompareEqual(Format::inf, one), "CompareEqual(+inf, 1) should be false");
  }

  static bool test_equal_finite()
  {
    const auto a = p3109::Encode<Format>(p3109::mpfr_float(2.0));
    const auto b = p3109::Encode<Format>(p3109::mpfr_float(2.0));
    return test_utils::expect_true(p3109::CompareEqual(a, b), "CompareEqual(2, 2) should be true");
  }

  static bool test_unequal_finite()
  {
    const auto a = p3109::Encode<Format>(p3109::mpfr_float(1.0));
    const auto b = p3109::Encode<Format>(p3109::mpfr_float(2.0));
    return test_utils::expect_true(!p3109::CompareEqual(a, b), "CompareEqual(1, 2) should be false");
  }

  static void run(test_utils::suite &s)
  {
    s.with_path(Format::name(), [&s] {
      s.run({"nan_lhs"}, test_nan_lhs());
      s.run({"nan_rhs"}, test_nan_rhs());
      s.run({"nan_both"}, test_nan_both());
      s.run({"posinf_equal"}, test_posinf_equal());
      s.run({"neginf_equal"}, test_neginf_equal());
      s.run({"posinf_vs_finite"}, test_posinf_vs_finite());
      s.run({"equal_finite"}, test_equal_finite());
      s.run({"unequal_finite"}, test_unequal_finite());
    });
  }
};

int main()
{
  test_utils::suite s{"compare_equal"};

  using Binary8p3se = p3109::binary<8, 3, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using Binary8p4se = p3109::binary<8, 4, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using Binary8p3uf = p3109::binary<8, 3, p3109::Signedness::Unsigned, p3109::Domain::Finite>;

  TestCompareEqual<Binary8p3se>::run(s);
  TestCompareEqual<Binary8p4se>::run(s);
  TestCompareEqual<Binary8p3uf>::run(s);

  return s.finalize();
}
