#include "test_utils.h"

// Tests for Subtract, Multiply, Divide, Negate, Recip, RSqrt, CopySign, Hypot
template <typename FormatX, typename FormatY, typename FormatR,
  typename ProjSpec = p3109::ProjectionSpec<p3109::NearestTiesToEven, p3109::SatFinite>>
struct TestArith {
  static void run(test_utils::suite &s)
  {
    ProjSpec ps{};
    std::string path = std::string(FormatX::name()) + "+" + FormatY::name() + "/" + FormatR::name();

    s.with_path(path + "/Subtract", [&] {
      s.run({"nan_lhs"}, p3109::Subtract<FormatR>(FormatX::nan, p3109::Encode<FormatY>(p3109::mpfr_float(1)), ps).isnan());
      s.run({"nan_rhs"}, p3109::Subtract<FormatR>(p3109::Encode<FormatX>(p3109::mpfr_float(1)), FormatY::nan, ps).isnan());
      s.run({"inf_inf"}, p3109::Subtract<FormatR>(FormatX::inf, FormatY::inf, ps).isnan());
      s.run({"inf_fin"}, p3109::Subtract<FormatR>(FormatX::inf, p3109::Encode<FormatY>(p3109::mpfr_float(1)), ps).isposinf());
      s.run({"finite"}, p3109::Subtract<FormatR>(p3109::Encode<FormatX>(p3109::mpfr_float(3)), p3109::Encode<FormatY>(p3109::mpfr_float(1)), ps).codepoint == p3109::Encode<FormatR>(p3109::mpfr_float(2)).codepoint);
    });

    s.with_path(path + "/Multiply", [&] {
      s.run({"nan_lhs"}, p3109::Multiply<FormatR>(FormatX::nan, p3109::Encode<FormatY>(p3109::mpfr_float(1)), ps).isnan());
      s.run({"nan_rhs"}, p3109::Multiply<FormatR>(p3109::Encode<FormatX>(p3109::mpfr_float(1)), FormatY::nan, ps).isnan());
      s.run({"inf_inf"}, p3109::Multiply<FormatR>(FormatX::inf, FormatY::inf, ps).isposinf());
      s.run({"inf_zero"}, p3109::Multiply<FormatR>(FormatX::inf, p3109::Encode<FormatY>(p3109::mpfr_float(0)), ps).isnan());
      s.run({"zero_inf"}, p3109::Multiply<FormatR>(p3109::Encode<FormatX>(p3109::mpfr_float(0)), FormatY::inf, ps).isnan());
      s.run({"finite"}, p3109::Multiply<FormatR>(p3109::Encode<FormatX>(p3109::mpfr_float(2)), p3109::Encode<FormatY>(p3109::mpfr_float(2)), ps).codepoint == p3109::Encode<FormatR>(p3109::mpfr_float(4)).codepoint);
    });

    s.with_path(path + "/Divide", [&] {
      s.run({"nan_lhs"}, p3109::Divide<FormatR>(FormatX::nan, p3109::Encode<FormatY>(p3109::mpfr_float(1)), ps).isnan());
      s.run({"nan_rhs"}, p3109::Divide<FormatR>(p3109::Encode<FormatX>(p3109::mpfr_float(1)), FormatY::nan, ps).isnan());
      s.run({"inf_inf"}, p3109::Divide<FormatR>(FormatX::inf, FormatY::inf, ps).isnan());
      s.run({"div_zero"}, p3109::Divide<FormatR>(p3109::Encode<FormatX>(p3109::mpfr_float(1)), p3109::Encode<FormatY>(p3109::mpfr_float(0)), ps).isnan());
      s.run({"inf_fin"}, p3109::Divide<FormatR>(FormatX::inf, p3109::Encode<FormatY>(p3109::mpfr_float(1)), ps).isposinf());
      s.run({"fin_inf"}, p3109::Divide<FormatR>(p3109::Encode<FormatX>(p3109::mpfr_float(1)), FormatY::inf, ps).codepoint == p3109::Encode<FormatR>(p3109::mpfr_float(0)).codepoint);
      s.run({"finite"}, p3109::Divide<FormatR>(p3109::Encode<FormatX>(p3109::mpfr_float(4)), p3109::Encode<FormatY>(p3109::mpfr_float(2)), ps).codepoint == p3109::Encode<FormatR>(p3109::mpfr_float(2)).codepoint);
    });

    s.with_path(path + "/Negate", [&] {
      s.run({"nan"}, p3109::Negate<FormatR>(FormatX::nan, ps).isnan());
      if constexpr (FormatX::is_signed)
      {
        s.run({"posinf"}, p3109::Negate<FormatR>(FormatX::inf, ps).isneginf());
        s.run({"neginf"}, p3109::Negate<FormatR>(FormatX::ninf, ps).isposinf());
        s.run({"finite"}, p3109::Negate<FormatR>(p3109::Encode<FormatX>(p3109::mpfr_float(2)), ps).codepoint == p3109::Encode<FormatR>(p3109::mpfr_float(-2)).codepoint);
      }
    });

    s.with_path(path + "/Recip", [&] {
      s.run({"nan"}, p3109::Recip<FormatR>(FormatX::nan, ps).isnan());
      s.run({"zero"}, p3109::Recip<FormatR>(p3109::Encode<FormatX>(p3109::mpfr_float(0)), ps).isnan());
      s.run({"inf"}, p3109::Recip<FormatR>(FormatX::inf, ps).codepoint == p3109::Encode<FormatR>(p3109::mpfr_float(0)).codepoint);
      s.run({"one"}, p3109::Recip<FormatR>(p3109::Encode<FormatX>(p3109::mpfr_float(1)), ps).codepoint == p3109::Encode<FormatR>(p3109::mpfr_float(1)).codepoint);
    });

    s.with_path(path + "/RSqrt", [&] {
      s.run({"nan"}, p3109::RSqrt<FormatR>(FormatX::nan, ps).isnan());
      if constexpr (FormatX::is_signed)
        s.run({"neginf"}, p3109::RSqrt<FormatR>(FormatX::ninf, ps).isnan());
      s.run({"zero"}, p3109::RSqrt<FormatR>(p3109::Encode<FormatX>(p3109::mpfr_float(0)), ps).isnan());
      s.run({"posinf"}, p3109::RSqrt<FormatR>(FormatX::inf, ps).codepoint == p3109::Encode<FormatR>(p3109::mpfr_float(0)).codepoint);
      s.run({"one"}, p3109::RSqrt<FormatR>(p3109::Encode<FormatX>(p3109::mpfr_float(1)), ps).codepoint == p3109::Encode<FormatR>(p3109::mpfr_float(1)).codepoint);
    });

    s.with_path(path + "/CopySign", [&] {
      s.run({"nan_x"}, p3109::CopySign<FormatR>(FormatX::nan, p3109::Encode<FormatY>(p3109::mpfr_float(1)), ps).isnan());
      s.run({"nan_y"}, p3109::CopySign<FormatR>(p3109::Encode<FormatX>(p3109::mpfr_float(1)), FormatY::nan, ps).isnan());
      if constexpr (FormatX::is_signed && FormatY::is_signed)
      {
        s.run({"pos_pos"}, p3109::CopySign<FormatR>(p3109::Encode<FormatX>(p3109::mpfr_float(2)), p3109::Encode<FormatY>(p3109::mpfr_float(1)), ps).codepoint == p3109::Encode<FormatR>(p3109::mpfr_float(2)).codepoint);
        s.run({"pos_neg"}, p3109::CopySign<FormatR>(p3109::Encode<FormatX>(p3109::mpfr_float(2)), p3109::Encode<FormatY>(p3109::mpfr_float(-2)), ps).codepoint == p3109::Encode<FormatR>(p3109::mpfr_float(-2)).codepoint);
        s.run({"neg_pos"}, p3109::CopySign<FormatR>(p3109::Encode<FormatX>(p3109::mpfr_float(-2)), p3109::Encode<FormatY>(p3109::mpfr_float(1)), ps).codepoint == p3109::Encode<FormatR>(p3109::mpfr_float(2)).codepoint);
      }
    });

    s.with_path(path + "/Hypot", [&] {
      s.run({"nan_x"}, p3109::Hypot<FormatR>(FormatX::nan, p3109::Encode<FormatY>(p3109::mpfr_float(1)), ps).isnan());
      s.run({"nan_y"}, p3109::Hypot<FormatR>(p3109::Encode<FormatX>(p3109::mpfr_float(1)), FormatY::nan, ps).isnan());
      s.run({"inf_x"}, p3109::Hypot<FormatR>(FormatX::inf, p3109::Encode<FormatY>(p3109::mpfr_float(1)), ps).isposinf());
      s.run({"inf_y"}, p3109::Hypot<FormatR>(p3109::Encode<FormatX>(p3109::mpfr_float(1)), FormatY::inf, ps).isposinf());
      // hypot(3,4) = 5
      s.run({"3_4"}, p3109::Hypot<FormatR>(p3109::Encode<FormatX>(p3109::mpfr_float(3)), p3109::Encode<FormatY>(p3109::mpfr_float(4)), ps).codepoint == p3109::Encode<FormatR>(p3109::mpfr_float(5)).codepoint);
    });
  }
};

int main()
{
  test_utils::suite s{"arith"};
  using F8p3 = p3109::binary<8, 3, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using F8p4 = p3109::binary<8, 4, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using F8u = p3109::binary<8, 3, p3109::Signedness::Unsigned, p3109::Domain::Finite>;
  // Same-format
  TestArith<F8p3, F8p3, F8p3>::run(s);
  TestArith<F8p4, F8p4, F8p4>::run(s);
  TestArith<F8u, F8u, F8u>::run(s);
  // Mixed-format (exercise FormatX, FormatY, FormatR independently)
  TestArith<F8p3, F8p4, F8p3>::run(s);
  TestArith<F8p3, F8p3, F8p4>::run(s);
  return s.finalize();
}
