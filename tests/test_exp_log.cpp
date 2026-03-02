#include "test_utils.h"

// Tests for Exp, Exp2, Log, Log2, Softplus, ExpMinusOne, LogOnePlus
template <typename Format, typename ProjSpec = p3109::ProjectionSpec<p3109::NearestTiesToEven, p3109::SatFinite>>
struct TestExpLog {
  static void run(test_utils::suite &s)
  {
    auto nan = Format::nan;
    auto inf = Format::inf;
    auto zero = p3109::Encode<Format>(p3109::mpfr_float(0));
    auto one = p3109::Encode<Format>(p3109::mpfr_float(1));
    ProjSpec ps{};

    s.with_path(std::string(Format::name()) + "/Exp", [&] {
      s.run({"nan"}, p3109::Exp<Format>(nan, ps).isnan());
      s.run({"posinf"}, p3109::Exp<Format>(inf, ps).isposinf());
      if constexpr (Format::is_signed)
        s.run({"neginf"}, p3109::Exp<Format>(Format::ninf, ps).codepoint == zero.codepoint);
      s.run({"zero"}, p3109::Exp<Format>(zero, ps).codepoint == one.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/Exp2", [&] {
      s.run({"nan"}, p3109::Exp2<Format>(nan, ps).isnan());
      s.run({"posinf"}, p3109::Exp2<Format>(inf, ps).isposinf());
      if constexpr (Format::is_signed)
        s.run({"neginf"}, p3109::Exp2<Format>(Format::ninf, ps).codepoint == zero.codepoint);
      s.run({"zero"}, p3109::Exp2<Format>(zero, ps).codepoint == one.codepoint);
      auto two = p3109::Encode<Format>(p3109::mpfr_float(2));
      s.run({"one"}, p3109::Exp2<Format>(one, ps).codepoint == two.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/Log", [&] {
      s.run({"nan"}, p3109::Log<Format>(nan, ps).isnan());
      s.run({"posinf"}, p3109::Log<Format>(inf, ps).isposinf());
      if constexpr (Format::is_signed)
        s.run({"neginf"}, p3109::Log<Format>(Format::ninf, ps).isnan());
      s.run({"one"}, p3109::Log<Format>(one, ps).codepoint == zero.codepoint);
      if constexpr (Format::is_signed)
        s.run({"zero"}, p3109::Log<Format>(zero, ps).isneginf());
    });

    s.with_path(std::string(Format::name()) + "/Log2", [&] {
      s.run({"nan"}, p3109::Log2<Format>(nan, ps).isnan());
      s.run({"posinf"}, p3109::Log2<Format>(inf, ps).isposinf());
      s.run({"one"}, p3109::Log2<Format>(one, ps).codepoint == zero.codepoint);
      if constexpr (Format::is_signed)
        s.run({"zero"}, p3109::Log2<Format>(zero, ps).isneginf());
      auto two = p3109::Encode<Format>(p3109::mpfr_float(2));
      s.run({"two"}, p3109::Log2<Format>(two, ps).codepoint == one.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/Softplus", [&] {
      s.run({"nan"}, p3109::Softplus<Format>(nan, ps).isnan());
      s.run({"posinf"}, p3109::Softplus<Format>(inf, ps).isposinf());
      if constexpr (Format::is_signed)
        s.run({"neginf"}, p3109::Softplus<Format>(Format::ninf, ps).codepoint == zero.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/ExpMinusOne", [&] {
      s.run({"nan"}, p3109::ExpMinusOne<Format>(nan, ps).isnan());
      s.run({"posinf"}, p3109::ExpMinusOne<Format>(inf, ps).isposinf());
      s.run({"zero"}, p3109::ExpMinusOne<Format>(zero, ps).codepoint == zero.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/LogOnePlus", [&] {
      s.run({"nan"}, p3109::LogOnePlus<Format>(nan, ps).isnan());
      s.run({"zero"}, p3109::LogOnePlus<Format>(zero, ps).codepoint == zero.codepoint);
    });
  }
};

int main()
{
  test_utils::suite s{"exp_log"};
  using F8p3 = p3109::binary<8, 3, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using F8p4 = p3109::binary<8, 4, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using F8u = p3109::binary<8, 3, p3109::Signedness::Unsigned, p3109::Domain::Finite>;
  TestExpLog<F8p3>::run(s);
  TestExpLog<F8p4>::run(s);
  TestExpLog<F8u>::run(s);
  return s.finalize();
}
