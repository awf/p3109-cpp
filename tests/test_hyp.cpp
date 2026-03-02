#include "test_utils.h"

// Tests for Sinh, Cosh, Tanh
template <typename Format, typename ProjSpec = p3109::ProjectionSpec<p3109::NearestTiesToEven, p3109::SatFinite>>
struct TestHyp {
  static void run(test_utils::suite &s)
  {
    auto nan = Format::nan;
    auto inf = Format::inf;
    auto zero = p3109::Encode<Format>(p3109::mpfr_float(0));
    auto one = p3109::Encode<Format>(p3109::mpfr_float(1));
    ProjSpec ps{};

    s.with_path(std::string(Format::name()) + "/Sinh", [&] {
      s.run({"nan"}, p3109::Sinh<Format>(nan, ps).isnan());
      s.run({"posinf"}, p3109::Sinh<Format>(inf, ps).isposinf());
      if constexpr (Format::is_signed)
        s.run({"neginf"}, p3109::Sinh<Format>(Format::ninf, ps).isneginf());
      s.run({"zero"}, p3109::Sinh<Format>(zero, ps).codepoint == zero.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/Cosh", [&] {
      s.run({"nan"}, p3109::Cosh<Format>(nan, ps).isnan());
      s.run({"posinf"}, p3109::Cosh<Format>(inf, ps).isposinf());
      if constexpr (Format::is_signed)
        s.run({"neginf"}, p3109::Cosh<Format>(Format::ninf, ps).isposinf());
      s.run({"zero"}, p3109::Cosh<Format>(zero, ps).codepoint == one.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/Tanh", [&] {
      s.run({"nan"}, p3109::Tanh<Format>(nan, ps).isnan());
      auto tanh_inf = p3109::Tanh<Format>(inf, ps);
      s.run({"posinf"}, tanh_inf.codepoint == one.codepoint);
      if constexpr (Format::is_signed)
      {
        auto neg1 = p3109::Encode<Format>(p3109::mpfr_float(-1));
        auto tanh_ninf = p3109::Tanh<Format>(Format::ninf, ps);
        s.run({"neginf"}, tanh_ninf.codepoint == neg1.codepoint);
      }
      s.run({"zero"}, p3109::Tanh<Format>(zero, ps).codepoint == zero.codepoint);
    });
  }
};

int main()
{
  test_utils::suite s{"hyp"};
  using F8p3 = p3109::binary<8, 3, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using F8p4 = p3109::binary<8, 4, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using F8u = p3109::binary<8, 3, p3109::Signedness::Unsigned, p3109::Domain::Finite>;
  TestHyp<F8p3>::run(s);
  TestHyp<F8p4>::run(s);
  TestHyp<F8u>::run(s);
  return s.finalize();
}
