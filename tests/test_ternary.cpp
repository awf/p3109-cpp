#include "test_utils.h"

// Tests for FAA, Clamp
template <typename Format, typename ProjSpec = p3109::ProjectionSpec<p3109::NearestTiesToEven, p3109::SatFinite>>
struct TestTernary {
  static void run(test_utils::suite &s)
  {
    auto nan = Format::nan;
    auto inf = Format::inf;
    auto zero = p3109::Encode<Format>(p3109::mpfr_float(0));
    auto one = p3109::Encode<Format>(p3109::mpfr_float(1));
    auto two = p3109::Encode<Format>(p3109::mpfr_float(2));
    auto three = p3109::Encode<Format>(p3109::mpfr_float(3));
    auto six = p3109::Encode<Format>(p3109::mpfr_float(6));
    ProjSpec ps{};

    s.with_path(std::string(Format::name()) + "/FAA", [&] {
      s.run({"nan_x"}, p3109::FAA<Format>(nan, one, two, ps).isnan());
      s.run({"nan_y"}, p3109::FAA<Format>(one, nan, two, ps).isnan());
      s.run({"nan_z"}, p3109::FAA<Format>(one, two, nan, ps).isnan());
      if constexpr (Format::is_signed)
        s.run({"inf_ninf"}, p3109::FAA<Format>(inf, Format::ninf, one, ps).isnan());
      s.run({"inf_inf_inf"}, p3109::FAA<Format>(inf, inf, inf, ps).isposinf());
      s.run({"finite"}, p3109::FAA<Format>(one, two, three, ps).codepoint == six.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/Clamp", [&] {
      s.run({"nan_x"}, p3109::Clamp<Format>(nan, one, three, ps).isnan());
      s.run({"nan_lo"}, p3109::Clamp<Format>(two, nan, three, ps).isnan());
      s.run({"nan_hi"}, p3109::Clamp<Format>(two, one, nan, ps).isnan());
      // Value within range: Clamp(2, 1, 3) = 2
      s.run({"in_range"}, p3109::Clamp<Format>(two, one, three, ps).codepoint == two.codepoint);
      // Value below range: Clamp(0, 1, 3) = 1
      s.run({"below"}, p3109::Clamp<Format>(zero, one, three, ps).codepoint == one.codepoint);
      // lo > hi → NaN
      s.run({"lo_gt_hi"}, p3109::Clamp<Format>(two, three, one, ps).isnan());
    });
  }
};

int main()
{
  test_utils::suite s{"ternary"};
  using F8p3 = p3109::binary<8, 3, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using F8p4 = p3109::binary<8, 4, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using F8u = p3109::binary<8, 3, p3109::Signedness::Unsigned, p3109::Domain::Finite>;
  TestTernary<F8p3>::run(s);
  TestTernary<F8p4>::run(s);
  TestTernary<F8u>::run(s);
  return s.finalize();
}
