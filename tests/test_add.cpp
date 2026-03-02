#include <string>

#include "test_utils.h"

// Add tests: binary projected operation exemplar
template <typename FormatX, typename FormatY, typename FormatR,
  typename ProjSpec = p3109::ProjectionSpec<p3109::NearestTiesToEven, p3109::SatFinite>>
struct TestAdd {
  static bool test_nan_lhs()
  {
    const auto z = p3109::Add<FormatR>(FormatX::nan, p3109::Encode<FormatY>(p3109::mpfr_float(1.0)), ProjSpec{});
    return test_utils::expect_true(z.isnan(), "Add(NaN, 1) should return NaN");
  }

  static bool test_nan_rhs()
  {
    const auto z = p3109::Add<FormatR>(p3109::Encode<FormatX>(p3109::mpfr_float(1.0)), FormatY::nan, ProjSpec{});
    return test_utils::expect_true(z.isnan(), "Add(1, NaN) should return NaN");
  }

  static bool test_inf_minus_inf()
  {
    if constexpr (FormatY::signedness == p3109::Signedness::Unsigned)
      return true;
    const auto z = p3109::Add<FormatR>(FormatX::inf, FormatY::ninf, ProjSpec{});
    return test_utils::expect_true(z.isnan(), "Add(+inf, -inf) should return NaN");
  }

  static bool test_posinf_lhs()
  {
    const auto z = p3109::Add<FormatR>(FormatX::inf, p3109::Encode<FormatY>(p3109::mpfr_float(1.0)), ProjSpec{});
    return test_utils::expect_true(z.isposinf(), "Add(+inf, 1) should return +inf");
  }

  static bool test_neginf_rhs()
  {
    if constexpr (FormatY::signedness == p3109::Signedness::Unsigned)
      return true;
    const auto z = p3109::Add<FormatR>(p3109::Encode<FormatX>(p3109::mpfr_float(1.0)), FormatY::ninf, ProjSpec{});
    return test_utils::expect_true(z.isneginf(), "Add(1, -inf) should return -inf");
  }

  static bool test_finite()
  {
    const auto x = p3109::Encode<FormatX>(p3109::mpfr_float(1.0));
    const auto y = p3109::Encode<FormatY>(p3109::mpfr_float(2.0));
    const auto z = p3109::Add<FormatR>(x, y, ProjSpec{});
    const auto expected = p3109::Encode<FormatR>(p3109::mpfr_float(3.0));
    return test_utils::expect_true(z.codepoint == expected.codepoint, "Add(1, 2) should return 3");
  }

  static void run(test_utils::suite &s)
  {
    s.with_path(std::string(FormatX::name()) + "+" + FormatY::name() + "/" + FormatR::name(), [&s] {
      s.run({"nan_lhs"}, test_nan_lhs());
      s.run({"nan_rhs"}, test_nan_rhs());
      s.run({"inf_minus_inf"}, test_inf_minus_inf());
      s.run({"posinf_lhs"}, test_posinf_lhs());
      s.run({"neginf_rhs"}, test_neginf_rhs());
      s.run({"finite"}, test_finite());
    });
  }
};

int main()
{
  test_utils::suite s{"add"};

  using Binary8p3se = p3109::binary<8, 3, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using Binary8p4se = p3109::binary<8, 4, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using Binary8p3uf = p3109::binary<8, 3, p3109::Signedness::Unsigned, p3109::Domain::Finite>;

  // Same-format
  TestAdd<Binary8p3se, Binary8p3se, Binary8p3se>::run(s);
  // Mixed-format
  TestAdd<Binary8p3se, Binary8p4se, Binary8p3se>::run(s);
  TestAdd<Binary8p3se, Binary8p3se, Binary8p4se>::run(s);
  // Unsigned
  TestAdd<Binary8p3uf, Binary8p3uf, Binary8p3uf>::run(s);

  return s.finalize();
}
