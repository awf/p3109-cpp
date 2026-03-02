#include "test_utils.h"

// Tests for ArcSin, ArcCos, ArcTan, ArcSinPi, ArcCosPi, ArcTanPi, ArcSinh, ArcCosh, ArcTanh
template <typename Format, typename ProjSpec = p3109::ProjectionSpec<p3109::NearestTiesToEven, p3109::SatFinite>>
struct TestInvTrig {
  static void run(test_utils::suite &s)
  {
    auto nan = Format::nan;
    auto inf = Format::inf;
    auto zero = p3109::Encode<Format>(p3109::mpfr_float(0));
    auto one = p3109::Encode<Format>(p3109::mpfr_float(1));
    ProjSpec ps{};

    s.with_path(std::string(Format::name()) + "/ArcSin", [&] {
      s.run({"nan"}, p3109::ArcSin<Format>(nan, ps).isnan());
      s.run({"posinf"}, p3109::ArcSin<Format>(inf, ps).isnan());
      s.run({"zero"}, p3109::ArcSin<Format>(zero, ps).codepoint == zero.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/ArcCos", [&] {
      s.run({"nan"}, p3109::ArcCos<Format>(nan, ps).isnan());
      s.run({"posinf"}, p3109::ArcCos<Format>(inf, ps).isnan());
      s.run({"one"}, !p3109::ArcCos<Format>(one, ps).isnan()); // arccos(1) = 0
    });

    s.with_path(std::string(Format::name()) + "/ArcTan", [&] {
      s.run({"nan"}, p3109::ArcTan<Format>(nan, ps).isnan());
      s.run({"posinf"}, !p3109::ArcTan<Format>(inf, ps).isnan()); // pi/2
      s.run({"zero"}, p3109::ArcTan<Format>(zero, ps).codepoint == zero.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/ArcSinPi", [&] {
      s.run({"nan"}, p3109::ArcSinPi<Format>(nan, ps).isnan());
      s.run({"posinf"}, p3109::ArcSinPi<Format>(inf, ps).isnan());
      s.run({"zero"}, p3109::ArcSinPi<Format>(zero, ps).codepoint == zero.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/ArcCosPi", [&] {
      s.run({"nan"}, p3109::ArcCosPi<Format>(nan, ps).isnan());
      s.run({"posinf"}, p3109::ArcCosPi<Format>(inf, ps).isnan());
    });

    s.with_path(std::string(Format::name()) + "/ArcTanPi", [&] {
      s.run({"nan"}, p3109::ArcTanPi<Format>(nan, ps).isnan());
      s.run({"zero"}, p3109::ArcTanPi<Format>(zero, ps).codepoint == zero.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/ArcSinh", [&] {
      s.run({"nan"}, p3109::ArcSinh<Format>(nan, ps).isnan());
      s.run({"posinf"}, p3109::ArcSinh<Format>(inf, ps).isposinf());
      if constexpr (Format::is_signed)
        s.run({"neginf"}, p3109::ArcSinh<Format>(Format::ninf, ps).isneginf());
      s.run({"zero"}, p3109::ArcSinh<Format>(zero, ps).codepoint == zero.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/ArcCosh", [&] {
      s.run({"nan"}, p3109::ArcCosh<Format>(nan, ps).isnan());
      s.run({"posinf"}, p3109::ArcCosh<Format>(inf, ps).isposinf());
      if constexpr (Format::is_signed)
        s.run({"neginf"}, p3109::ArcCosh<Format>(Format::ninf, ps).isnan());
      s.run({"one"}, p3109::ArcCosh<Format>(one, ps).codepoint == zero.codepoint); // acosh(1)=0
    });

    s.with_path(std::string(Format::name()) + "/ArcTanh", [&] {
      s.run({"nan"}, p3109::ArcTanh<Format>(nan, ps).isnan());
      s.run({"posinf"}, p3109::ArcTanh<Format>(inf, ps).isnan());
      s.run({"one"}, p3109::ArcTanh<Format>(one, ps).isposinf());
      if constexpr (Format::is_signed)
      {
        auto neg1 = p3109::Encode<Format>(p3109::mpfr_float(-1));
        s.run({"neg1"}, p3109::ArcTanh<Format>(neg1, ps).isneginf());
      }
      s.run({"zero"}, p3109::ArcTanh<Format>(zero, ps).codepoint == zero.codepoint);
    });
  }
};

int main()
{
  test_utils::suite s{"inv_trig"};
  using F8p3 = p3109::binary<8, 3, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using F8p4 = p3109::binary<8, 4, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using F8u = p3109::binary<8, 3, p3109::Signedness::Unsigned, p3109::Domain::Finite>;
  TestInvTrig<F8p3>::run(s);
  TestInvTrig<F8p4>::run(s);
  TestInvTrig<F8u>::run(s);
  return s.finalize();
}
