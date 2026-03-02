#include <string>

#include "test_utils.h"

// FMA tests: ternary projected operation exemplar
template <typename Format, typename ProjSpec = p3109::ProjectionSpec<p3109::NearestTiesToEven, p3109::SatFinite>>
struct TestFMA {
  static bool test_nan_x()
  {
    const auto one = p3109::Encode<Format>(p3109::mpfr_float(1.0));
    const auto z = p3109::FMA<Format>(Format::nan, one, one, ProjSpec{});
    return test_utils::expect_true(z.isnan(), "FMA(NaN, 1, 1) should return NaN");
  }

  static bool test_nan_y()
  {
    const auto one = p3109::Encode<Format>(p3109::mpfr_float(1.0));
    const auto z = p3109::FMA<Format>(one, Format::nan, one, ProjSpec{});
    return test_utils::expect_true(z.isnan(), "FMA(1, NaN, 1) should return NaN");
  }

  static bool test_nan_z()
  {
    const auto one = p3109::Encode<Format>(p3109::mpfr_float(1.0));
    const auto z = p3109::FMA<Format>(one, one, Format::nan, ProjSpec{});
    return test_utils::expect_true(z.isnan(), "FMA(1, 1, NaN) should return NaN");
  }

  static bool test_zero_times_inf()
  {
    const auto zero = p3109::Encode<Format>(p3109::mpfr_float(0.0));
    const auto one = p3109::Encode<Format>(p3109::mpfr_float(1.0));
    const auto z = p3109::FMA<Format>(zero, Format::inf, one, ProjSpec{});
    return test_utils::expect_true(z.isnan(), "FMA(0, +inf, 1) should return NaN");
  }

  static bool test_inf_times_zero()
  {
    const auto zero = p3109::Encode<Format>(p3109::mpfr_float(0.0));
    const auto one = p3109::Encode<Format>(p3109::mpfr_float(1.0));
    const auto z = p3109::FMA<Format>(Format::inf, zero, one, ProjSpec{});
    return test_utils::expect_true(z.isnan(), "FMA(+inf, 0, 1) should return NaN");
  }

  static bool test_inf_product_opposite_inf_addend()
  {
    if constexpr (Format::signedness == p3109::Signedness::Unsigned)
      return true;
    // +inf * +inf = +inf, addend = -inf => NaN
    const auto one = p3109::Encode<Format>(p3109::mpfr_float(1.0));
    const auto z = p3109::FMA<Format>(Format::inf, Format::inf, Format::ninf, ProjSpec{});
    return test_utils::expect_true(z.isnan(), "FMA(+inf, +inf, -inf) should return NaN");
  }

  static bool test_inf_product_same_inf_addend()
  {
    // +inf * +inf = +inf, addend = +inf => +inf
    const auto z = p3109::FMA<Format>(Format::inf, Format::inf, Format::inf, ProjSpec{});
    return test_utils::expect_true(z.isposinf(), "FMA(+inf, +inf, +inf) should return +inf");
  }

  static bool test_inf_times_finite()
  {
    const auto two = p3109::Encode<Format>(p3109::mpfr_float(2.0));
    const auto one = p3109::Encode<Format>(p3109::mpfr_float(1.0));
    const auto z = p3109::FMA<Format>(Format::inf, two, one, ProjSpec{});
    return test_utils::expect_true(z.isposinf(), "FMA(+inf, 2, 1) should return +inf");
  }

  static bool test_finite_plus_inf()
  {
    const auto two = p3109::Encode<Format>(p3109::mpfr_float(2.0));
    const auto three = p3109::Encode<Format>(p3109::mpfr_float(3.0));
    const auto z = p3109::FMA<Format>(two, three, Format::inf, ProjSpec{});
    return test_utils::expect_true(z.isposinf(), "FMA(2, 3, +inf) should return +inf");
  }

  static bool test_finite()
  {
    const auto two = p3109::Encode<Format>(p3109::mpfr_float(2.0));
    const auto three = p3109::Encode<Format>(p3109::mpfr_float(3.0));
    const auto one = p3109::Encode<Format>(p3109::mpfr_float(1.0));
    const auto z = p3109::FMA<Format>(two, three, one, ProjSpec{});
    // 2*3+1 = 7
    const auto expected = p3109::Encode<Format>(p3109::mpfr_float(7.0));
    return test_utils::expect_true(z.codepoint == expected.codepoint, "FMA(2, 3, 1) should return 7");
  }

  static void run(test_utils::suite &s)
  {
    s.with_path(Format::name(), [&s] {
      s.run({"nan_x"}, test_nan_x());
      s.run({"nan_y"}, test_nan_y());
      s.run({"nan_z"}, test_nan_z());
      s.run({"zero_times_inf"}, test_zero_times_inf());
      s.run({"inf_times_zero"}, test_inf_times_zero());
      s.run({"inf_product_opposite_inf_addend"}, test_inf_product_opposite_inf_addend());
      s.run({"inf_product_same_inf_addend"}, test_inf_product_same_inf_addend());
      s.run({"inf_times_finite"}, test_inf_times_finite());
      s.run({"finite_plus_inf"}, test_finite_plus_inf());
      s.run({"finite"}, test_finite());
    });
  }
};

int main()
{
  test_utils::suite s{"fma"};

  using Binary8p3se = p3109::binary<8, 3, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using Binary8p4se = p3109::binary<8, 4, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using Binary8p3uf = p3109::binary<8, 3, p3109::Signedness::Unsigned, p3109::Domain::Finite>;

  TestFMA<Binary8p3se>::run(s);
  TestFMA<Binary8p4se>::run(s);
  TestFMA<Binary8p3uf>::run(s);

  return s.finalize();
}
