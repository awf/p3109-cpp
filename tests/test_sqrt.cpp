#include <string>

#include <tuple>

#include "test_utils.h"

template <typename FormatX, typename FormatR,
  typename ProjSpec = p3109::ProjectionSpec<p3109::NearestTiesToEven, p3109::SatFinite>>
struct TestSqrt {
  static bool test_nan()
  {
    const auto z = p3109::Sqrt<FormatR>(FormatX::nan, ProjSpec{});
    return test_utils::expect_true(z.isnan(), "Sqrt(NaN) should return NaN");
  }

  static bool test_neginf()
  {
    if constexpr (FormatX::signedness == p3109::Unsigned)
      return true;
    const auto z = p3109::Sqrt<FormatR>(FormatX::ninf, ProjSpec{});
    return test_utils::expect_true(z.isnan(), "Sqrt(-inf) should return NaN");
  }

  static bool test_negative()
  {
    if constexpr (FormatX::signedness == p3109::Unsigned)
      return true;
    const auto x = p3109::Encode<FormatX>(p3109::mpfr_float(-1.0));
    const auto z = p3109::Sqrt<FormatR>(x, ProjSpec{});
    return test_utils::expect_true(z.isnan(), "Sqrt(X) with X<0 should return NaN");
  }

  static bool test_posinf()
  {
    const auto z = p3109::Sqrt<FormatR>(FormatX::inf, ProjSpec{});
    return test_utils::expect_true(z.isposinf(), "Sqrt(+inf) should return +inf");
  }

  static bool test_positive()
  {
    const auto x = p3109::Encode<FormatX>(p3109::mpfr_float(4.0));
    const auto z = p3109::Sqrt<FormatR>(x, ProjSpec{});
    const auto expected = p3109::Encode<FormatR>(p3109::mpfr_float(2.0));
    return test_utils::expect_true(z.codepoint == expected.codepoint, "Sqrt(4) should return 2");
  }

  static void run(test_utils::suite &s)
  {
    s.with_path(std::string(FormatX::name()) + "/" + FormatR::name(), [&s] {
      s.run({"nan"}, test_nan());
      s.run({"neginf"}, test_neginf());
      s.run({"negative"}, test_negative());
      s.run({"posinf"}, test_posinf());
      s.run({"positive"}, test_positive());
    });
  }
};

template <typename FormatX, typename... FormatRs>
void run_sqrt_for_rhs(test_utils::suite &s)
{
  (TestSqrt<FormatX, FormatRs>::run(s), ...);
}

template <typename... FormatXs, typename... FormatRs>
void run_sqrt_cross_product(test_utils::suite &s, std::tuple<FormatXs...>, std::tuple<FormatRs...>)
{
  (run_sqrt_for_rhs<FormatXs, FormatRs...>(s), ...);
}

int main()
{
  test_utils::suite s{"sqrt"};

  using Binary8p3se = p3109::binary<8, 3, p3109::Signed, p3109::Extended>;
  using Binary8p4se = p3109::binary<8, 4, p3109::Signed, p3109::Extended>;
  using Binary8p3sf = p3109::binary<8, 3, p3109::Signed, p3109::Finite>;
  using Binary8p4sf = p3109::binary<8, 4, p3109::Signed, p3109::Finite>;
  using Binary8p3uf = p3109::binary<8, 3, p3109::Unsigned, p3109::Finite>;
  using Binary8p4uf = p3109::binary<8, 4, p3109::Unsigned, p3109::Finite>;

  using AllFormats = std::tuple<Binary8p3se, Binary8p4se, Binary8p3sf, Binary8p4sf, Binary8p3uf, Binary8p4uf>;
  run_sqrt_cross_product(s, AllFormats{}, AllFormats{});

  using RTZF = p3109::ProjectionSpec<p3109::TowardZero, p3109::SatFinite>;
  TestSqrt<Binary8p3se, Binary8p3se, RTZF>::run(s);

  // Single-format stochastic test
  s.with_path("stochastic", [&s] {
    using SR = p3109::ProjectionSpec<p3109::StochasticB<8>, p3109::SatFinite>;
    const auto x = p3109::Encode<Binary8p4se>(p3109::mpfr_float(4.0));
    unsigned random_bits = 7;
    const auto z = p3109::Sqrt<Binary8p4se>(x, SR{{random_bits}});
    const auto expected = p3109::Encode<Binary8p4se>(p3109::mpfr_float(2.0));
    return test_utils::expect_true(z.codepoint == expected.codepoint, "Sqrt(4) should return 2");
  });

  return s.finalize();
}
