#include "test_utils.h"

// Tests for ArcTan2, ArcTan2Alt, ArcTan2Pi
template <typename Format, typename ProjSpec = p3109::ProjectionSpec<p3109::NearestTiesToEven, p3109::SatFinite>>
struct TestAtan2 {
  static void run(test_utils::suite &s)
  {
    auto nan = Format::nan;
    auto inf = Format::inf;
    auto zero = p3109::Encode<Format>(p3109::mpfr_float(0));
    auto one = p3109::Encode<Format>(p3109::mpfr_float(1));
    ProjSpec ps{};

    s.with_path(std::string(Format::name()) + "/ArcTan2", [&] {
      s.run({"nan_y"}, p3109::ArcTan2<Format>(nan, one, ps).isnan());
      s.run({"nan_x"}, p3109::ArcTan2<Format>(one, nan, ps).isnan());
      s.run({"0_0"}, p3109::ArcTan2<Format>(zero, zero, ps).isnan());
      s.run({"inf_inf"}, p3109::ArcTan2<Format>(inf, inf, ps).isnan());
      s.run({"y_posinf"}, p3109::ArcTan2<Format>(one, inf, ps).codepoint == zero.codepoint);
      s.run({"posinf_x"}, !p3109::ArcTan2<Format>(inf, one, ps).isnan()); // pi/2
      s.run({"1_1"}, !p3109::ArcTan2<Format>(one, one, ps).isnan());      // pi/4
    });

    s.with_path(std::string(Format::name()) + "/ArcTan2Alt", [&] {
      s.run({"nan_y"}, p3109::ArcTan2Alt<Format>(nan, one, ps).isnan());
      s.run({"nan_x"}, p3109::ArcTan2Alt<Format>(one, nan, ps).isnan());
      s.run({"0_0"}, p3109::ArcTan2Alt<Format>(zero, zero, ps).codepoint == zero.codepoint);
      s.run({"0_posinf"}, p3109::ArcTan2Alt<Format>(zero, inf, ps).codepoint == zero.codepoint);
      s.run({"1_1"}, !p3109::ArcTan2Alt<Format>(one, one, ps).isnan()); // pi/4
    });

    s.with_path(std::string(Format::name()) + "/ArcTan2Pi", [&] {
      s.run({"nan_y"}, p3109::ArcTan2Pi<Format>(nan, one, ps).isnan());
      s.run({"nan_x"}, p3109::ArcTan2Pi<Format>(one, nan, ps).isnan());
      s.run({"0_0"}, p3109::ArcTan2Pi<Format>(zero, zero, ps).isnan());
      s.run({"inf_inf"}, p3109::ArcTan2Pi<Format>(inf, inf, ps).isnan());
      s.run({"y_posinf"}, p3109::ArcTan2Pi<Format>(one, inf, ps).codepoint == zero.codepoint);
    });
  }
};

int main()
{
  test_utils::suite s{"atan2"};
  using F8p3 = p3109::binary<8, 3, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using F8p4 = p3109::binary<8, 4, p3109::Signedness::Signed, p3109::Domain::Extended>;
  TestAtan2<F8p3>::run(s);
  TestAtan2<F8p4>::run(s);
  return s.finalize();
}
