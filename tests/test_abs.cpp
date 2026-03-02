#include <string>
#include <tuple>

#include "test_utils.h"

// Abs tests: unary projected operation exemplar
template <typename FormatX, typename FormatR,
  typename ProjSpec = p3109::ProjectionSpec<p3109::NearestTiesToEven, p3109::SatFinite>>
struct TestAbs {
  static bool test_nan()
  {
    const auto z = p3109::Abs<FormatR>(FormatX::nan, ProjSpec{});
    return test_utils::expect_true(z.isnan(), "Abs(NaN) should return NaN");
  }

  static bool test_posinf()
  {
    const auto z = p3109::Abs<FormatR>(FormatX::inf, ProjSpec{});
    return test_utils::expect_true(z.isposinf(), "Abs(+inf) should return +inf");
  }

  static bool test_neginf()
  {
    if constexpr (FormatX::signedness == p3109::Signedness::Unsigned)
      return true;
    const auto z = p3109::Abs<FormatR>(FormatX::ninf, ProjSpec{});
    return test_utils::expect_true(z.isposinf(), "Abs(-inf) should return +inf");
  }

  static bool test_positive()
  {
    const auto x = p3109::Encode<FormatX>(p3109::mpfr_float(4.0));
    const auto z = p3109::Abs<FormatR>(x, ProjSpec{});
    const auto expected = p3109::Encode<FormatR>(p3109::mpfr_float(4.0));
    return test_utils::expect_true(z.codepoint == expected.codepoint, "Abs(4) should return 4");
  }

  static bool test_negative()
  {
    if constexpr (FormatX::signedness == p3109::Signedness::Unsigned)
      return true;
    const auto x = p3109::Encode<FormatX>(p3109::mpfr_float(-4.0));
    const auto z = p3109::Abs<FormatR>(x, ProjSpec{});
    const auto expected = p3109::Encode<FormatR>(p3109::mpfr_float(4.0));
    return test_utils::expect_true(z.codepoint == expected.codepoint, "Abs(-4) should return 4");
  }

  static void run(test_utils::suite &s)
  {
    s.with_path(std::string(FormatX::name()) + "/" + FormatR::name(), [&s] {
      s.run({"nan"}, test_nan());
      s.run({"posinf"}, test_posinf());
      s.run({"neginf"}, test_neginf());
      s.run({"positive"}, test_positive());
      s.run({"negative"}, test_negative());
    });
  }
};

int main()
{
  test_utils::suite s{"abs"};

  using Binary8p3se = p3109::binary<8, 3, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using Binary8p4se = p3109::binary<8, 4, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using Binary8p3uf = p3109::binary<8, 3, p3109::Signedness::Unsigned, p3109::Domain::Finite>;

  TestAbs<Binary8p3se, Binary8p3se>::run(s);
  TestAbs<Binary8p3se, Binary8p4se>::run(s);
  TestAbs<Binary8p4se, Binary8p3se>::run(s);
  TestAbs<Binary8p3uf, Binary8p3uf>::run(s);

  return s.finalize();
}
