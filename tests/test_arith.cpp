#include "test_utils.h"

// Tests for Subtract, Multiply, Divide, Negate, Recip, RSqrt, CopySign, Hypot
template <typename Format, typename ProjSpec = p3109::ProjectionSpec<p3109::NearestTiesToEven, p3109::SatFinite>>
struct TestArith {
  static void run(test_utils::suite &s)
  {
    auto nan = Format::nan;
    auto inf = Format::inf;
    auto zero = p3109::Encode<Format>(p3109::mpfr_float(0));
    auto one = p3109::Encode<Format>(p3109::mpfr_float(1));
    auto two = p3109::Encode<Format>(p3109::mpfr_float(2));
    auto three = p3109::Encode<Format>(p3109::mpfr_float(3));
    auto four = p3109::Encode<Format>(p3109::mpfr_float(4));
    ProjSpec ps{};

    s.with_path(std::string(Format::name()) + "/Subtract", [&] {
      s.run({"nan_lhs"}, p3109::Subtract<Format>(nan, one, ps).isnan());
      s.run({"nan_rhs"}, p3109::Subtract<Format>(one, nan, ps).isnan());
      s.run({"inf_inf"}, p3109::Subtract<Format>(inf, inf, ps).isnan());
      s.run({"inf_fin"}, p3109::Subtract<Format>(inf, one, ps).isposinf());
      s.run({"finite"}, p3109::Subtract<Format>(three, one, ps).codepoint == two.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/Multiply", [&] {
      s.run({"nan_lhs"}, p3109::Multiply<Format>(nan, one, ps).isnan());
      s.run({"nan_rhs"}, p3109::Multiply<Format>(one, nan, ps).isnan());
      s.run({"inf_inf"}, p3109::Multiply<Format>(inf, inf, ps).isposinf());
      s.run({"inf_zero"}, p3109::Multiply<Format>(inf, zero, ps).isnan());
      s.run({"zero_inf"}, p3109::Multiply<Format>(zero, inf, ps).isnan());
      s.run({"finite"}, p3109::Multiply<Format>(two, two, ps).codepoint == four.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/Divide", [&] {
      s.run({"nan_lhs"}, p3109::Divide<Format>(nan, one, ps).isnan());
      s.run({"nan_rhs"}, p3109::Divide<Format>(one, nan, ps).isnan());
      s.run({"inf_inf"}, p3109::Divide<Format>(inf, inf, ps).isnan());
      s.run({"div_zero"}, p3109::Divide<Format>(one, zero, ps).isnan());
      s.run({"inf_fin"}, p3109::Divide<Format>(inf, one, ps).isposinf());
      s.run({"fin_inf"}, p3109::Divide<Format>(one, inf, ps).codepoint == zero.codepoint);
      s.run({"finite"}, p3109::Divide<Format>(four, two, ps).codepoint == two.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/Negate", [&] {
      s.run({"nan"}, p3109::Negate<Format>(nan, ps).isnan());
      if constexpr (Format::is_signed)
      {
        s.run({"posinf"}, p3109::Negate<Format>(inf, ps).isneginf());
        s.run({"neginf"}, p3109::Negate<Format>(Format::ninf, ps).isposinf());
        auto neg2 = p3109::Encode<Format>(p3109::mpfr_float(-2));
        s.run({"finite"}, p3109::Negate<Format>(two, ps).codepoint == neg2.codepoint);
      }
    });

    s.with_path(std::string(Format::name()) + "/Recip", [&] {
      s.run({"nan"}, p3109::Recip<Format>(nan, ps).isnan());
      s.run({"zero"}, p3109::Recip<Format>(zero, ps).isnan());
      s.run({"inf"}, p3109::Recip<Format>(inf, ps).codepoint == zero.codepoint);
      s.run({"one"}, p3109::Recip<Format>(one, ps).codepoint == one.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/RSqrt", [&] {
      s.run({"nan"}, p3109::RSqrt<Format>(nan, ps).isnan());
      if constexpr (Format::is_signed)
        s.run({"neginf"}, p3109::RSqrt<Format>(Format::ninf, ps).isnan());
      s.run({"zero"}, p3109::RSqrt<Format>(zero, ps).isnan());
      s.run({"posinf"}, p3109::RSqrt<Format>(inf, ps).codepoint == zero.codepoint);
      s.run({"one"}, p3109::RSqrt<Format>(one, ps).codepoint == one.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/CopySign", [&] {
      s.run({"nan_x"}, p3109::CopySign<Format>(nan, one, ps).isnan());
      s.run({"nan_y"}, p3109::CopySign<Format>(one, nan, ps).isnan());
      if constexpr (Format::is_signed)
      {
        auto neg2 = p3109::Encode<Format>(p3109::mpfr_float(-2));
        s.run({"pos_pos"}, p3109::CopySign<Format>(two, one, ps).codepoint == two.codepoint);
        s.run({"pos_neg"}, p3109::CopySign<Format>(two, neg2, ps).codepoint == neg2.codepoint);
        s.run({"neg_pos"}, p3109::CopySign<Format>(neg2, one, ps).codepoint == two.codepoint);
      }
    });

    s.with_path(std::string(Format::name()) + "/Hypot", [&] {
      s.run({"nan_x"}, p3109::Hypot<Format>(nan, one, ps).isnan());
      s.run({"nan_y"}, p3109::Hypot<Format>(one, nan, ps).isnan());
      s.run({"inf_x"}, p3109::Hypot<Format>(inf, one, ps).isposinf());
      s.run({"inf_y"}, p3109::Hypot<Format>(one, inf, ps).isposinf());
      // hypot(3,4) = 5
      auto five = p3109::Encode<Format>(p3109::mpfr_float(5));
      s.run({"3_4"}, p3109::Hypot<Format>(three, four, ps).codepoint == five.codepoint);
    });
  }
};

int main()
{
  test_utils::suite s{"arith"};
  using F8p3 = p3109::binary<8, 3, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using F8p4 = p3109::binary<8, 4, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using F8u = p3109::binary<8, 3, p3109::Signedness::Unsigned, p3109::Domain::Finite>;
  TestArith<F8p3>::run(s);
  TestArith<F8p4>::run(s);
  TestArith<F8u>::run(s);
  return s.finalize();
}
