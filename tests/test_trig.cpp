#include "test_utils.h"

// Tests for Sin, Cos, Tan, SinPi, CosPi, TanPi
template <typename Format, typename ProjSpec = p3109::ProjectionSpec<p3109::NearestTiesToEven, p3109::SatFinite>>
struct TestTrig {
  static void run(test_utils::suite &s)
  {
    auto nan = Format::nan;
    auto inf = Format::inf;
    auto zero = p3109::Encode<Format>(p3109::mpfr_float(0));
    ProjSpec ps{};

    s.with_path(std::string(Format::name()) + "/Sin", [&] {
      s.run({"nan"}, p3109::Sin<Format>(nan, ps).isnan());
      s.run({"posinf"}, p3109::Sin<Format>(inf, ps).isnan());
      if constexpr (Format::is_signed)
        s.run({"neginf"}, p3109::Sin<Format>(Format::ninf, ps).isnan());
      s.run({"zero"}, p3109::Sin<Format>(zero, ps).codepoint == zero.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/Cos", [&] {
      s.run({"nan"}, p3109::Cos<Format>(nan, ps).isnan());
      s.run({"posinf"}, p3109::Cos<Format>(inf, ps).isnan());
      auto cos0 = p3109::Cos<Format>(zero, ps);
      auto one = p3109::Encode<Format>(p3109::mpfr_float(1));
      s.run({"zero"}, cos0.codepoint == one.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/Tan", [&] {
      s.run({"nan"}, p3109::Tan<Format>(nan, ps).isnan());
      s.run({"posinf"}, p3109::Tan<Format>(inf, ps).isnan());
      s.run({"zero"}, p3109::Tan<Format>(zero, ps).codepoint == zero.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/SinPi", [&] {
      s.run({"nan"}, p3109::SinPi<Format>(nan, ps).isnan());
      s.run({"posinf"}, p3109::SinPi<Format>(inf, ps).isnan());
      s.run({"zero"}, p3109::SinPi<Format>(zero, ps).codepoint == zero.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/CosPi", [&] {
      s.run({"nan"}, p3109::CosPi<Format>(nan, ps).isnan());
      s.run({"posinf"}, p3109::CosPi<Format>(inf, ps).isnan());
      auto cospi0 = p3109::CosPi<Format>(zero, ps);
      auto one = p3109::Encode<Format>(p3109::mpfr_float(1));
      s.run({"zero"}, cospi0.codepoint == one.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/TanPi", [&] {
      s.run({"nan"}, p3109::TanPi<Format>(nan, ps).isnan());
      s.run({"posinf"}, p3109::TanPi<Format>(inf, ps).isnan());
      s.run({"zero"}, p3109::TanPi<Format>(zero, ps).codepoint == zero.codepoint);
    });
  }
};

int main()
{
  test_utils::suite s{"trig"};
  using F8p3 = p3109::binary<8, 3, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using F8p4 = p3109::binary<8, 4, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using F8u = p3109::binary<8, 3, p3109::Signedness::Unsigned, p3109::Domain::Finite>;
  TestTrig<F8p3>::run(s);
  TestTrig<F8p4>::run(s);
  TestTrig<F8u>::run(s);
  return s.finalize();
}
