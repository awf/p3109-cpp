#include "test_utils.h"

// Tests for Maximum, Minimum, MaximumFinite, MinimumFinite,
//          MaximumMagnitude, MinimumMagnitude, MaximumNumber, MinimumNumber,
//          MaximumMagnitudeNumber, MinimumMagnitudeNumber
template <typename Format, typename ProjSpec = p3109::ProjectionSpec<p3109::NearestTiesToEven, p3109::SatFinite>>
struct TestMinMax {
  static void run(test_utils::suite &s)
  {
    auto nan = Format::nan;
    auto inf = Format::inf;
    auto one = p3109::Encode<Format>(p3109::mpfr_float(1));
    auto two = p3109::Encode<Format>(p3109::mpfr_float(2));
    auto three = p3109::Encode<Format>(p3109::mpfr_float(3));
    ProjSpec ps{};

    s.with_path(std::string(Format::name()) + "/Maximum", [&] {
      s.run({"nan_x"}, p3109::Maximum<Format>(nan, one, ps).isnan());
      s.run({"nan_y"}, p3109::Maximum<Format>(one, nan, ps).isnan());
      s.run({"inf_inf"}, p3109::Maximum<Format>(inf, inf, ps).isposinf());
      s.run({"inf_fin"}, p3109::Maximum<Format>(inf, one, ps).isposinf());
      s.run({"finite"}, p3109::Maximum<Format>(one, three, ps).codepoint == three.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/Minimum", [&] {
      s.run({"nan_x"}, p3109::Minimum<Format>(nan, one, ps).isnan());
      s.run({"nan_y"}, p3109::Minimum<Format>(one, nan, ps).isnan());
      s.run({"inf_inf"}, p3109::Minimum<Format>(inf, inf, ps).isposinf());
      s.run({"finite"}, p3109::Minimum<Format>(one, three, ps).codepoint == one.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/MaximumFinite", [&] {
      s.run({"nan_nan"}, p3109::MaximumFinite<Format>(nan, nan, ps).isnan());
      s.run({"nan_y"}, p3109::MaximumFinite<Format>(nan, two, ps).codepoint == two.codepoint);
      s.run({"x_nan"}, p3109::MaximumFinite<Format>(two, nan, ps).codepoint == two.codepoint);
      s.run({"inf_fin"}, p3109::MaximumFinite<Format>(inf, two, ps).codepoint == two.codepoint);
      s.run({"finite"}, p3109::MaximumFinite<Format>(one, three, ps).codepoint == three.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/MinimumFinite", [&] {
      s.run({"nan_nan"}, p3109::MinimumFinite<Format>(nan, nan, ps).isnan());
      s.run({"nan_y"}, p3109::MinimumFinite<Format>(nan, two, ps).codepoint == two.codepoint);
      s.run({"x_nan"}, p3109::MinimumFinite<Format>(two, nan, ps).codepoint == two.codepoint);
      s.run({"inf_fin"}, p3109::MinimumFinite<Format>(inf, two, ps).codepoint == two.codepoint);
      s.run({"finite"}, p3109::MinimumFinite<Format>(one, three, ps).codepoint == one.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/MaximumMagnitude", [&] {
      s.run({"nan_x"}, p3109::MaximumMagnitude<Format>(nan, one, ps).isnan());
      s.run({"nan_y"}, p3109::MaximumMagnitude<Format>(one, nan, ps).isnan());
      s.run({"inf"}, p3109::MaximumMagnitude<Format>(inf, one, ps).isposinf());
      s.run({"finite"}, p3109::MaximumMagnitude<Format>(one, three, ps).codepoint == three.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/MinimumMagnitude", [&] {
      s.run({"nan_x"}, p3109::MinimumMagnitude<Format>(nan, one, ps).isnan());
      s.run({"nan_y"}, p3109::MinimumMagnitude<Format>(one, nan, ps).isnan());
      s.run({"inf_fin"}, p3109::MinimumMagnitude<Format>(inf, one, ps).codepoint == one.codepoint);
      s.run({"finite"}, p3109::MinimumMagnitude<Format>(one, three, ps).codepoint == one.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/MaximumNumber", [&] {
      s.run({"nan_nan"}, p3109::MaximumNumber<Format>(nan, nan, ps).isnan());
      s.run({"nan_y"}, p3109::MaximumNumber<Format>(nan, two, ps).codepoint == two.codepoint);
      s.run({"x_nan"}, p3109::MaximumNumber<Format>(two, nan, ps).codepoint == two.codepoint);
      s.run({"finite"}, p3109::MaximumNumber<Format>(one, three, ps).codepoint == three.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/MinimumNumber", [&] {
      s.run({"nan_nan"}, p3109::MinimumNumber<Format>(nan, nan, ps).isnan());
      s.run({"nan_y"}, p3109::MinimumNumber<Format>(nan, two, ps).codepoint == two.codepoint);
      s.run({"x_nan"}, p3109::MinimumNumber<Format>(two, nan, ps).codepoint == two.codepoint);
      s.run({"finite"}, p3109::MinimumNumber<Format>(one, three, ps).codepoint == one.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/MaximumMagnitudeNumber", [&] {
      s.run({"nan_nan"}, p3109::MaximumMagnitudeNumber<Format>(nan, nan, ps).isnan());
      s.run({"nan_y"}, p3109::MaximumMagnitudeNumber<Format>(nan, two, ps).codepoint == two.codepoint);
      s.run({"finite"}, p3109::MaximumMagnitudeNumber<Format>(one, three, ps).codepoint == three.codepoint);
    });

    s.with_path(std::string(Format::name()) + "/MinimumMagnitudeNumber", [&] {
      s.run({"nan_nan"}, p3109::MinimumMagnitudeNumber<Format>(nan, nan, ps).isnan());
      s.run({"nan_y"}, p3109::MinimumMagnitudeNumber<Format>(nan, two, ps).codepoint == two.codepoint);
      s.run({"finite"}, p3109::MinimumMagnitudeNumber<Format>(one, three, ps).codepoint == one.codepoint);
    });
  }
};

int main()
{
  test_utils::suite s{"minmax"};
  using F8p3 = p3109::binary<8, 3, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using F8p4 = p3109::binary<8, 4, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using F8u = p3109::binary<8, 3, p3109::Signedness::Unsigned, p3109::Domain::Finite>;
  TestMinMax<F8p3>::run(s);
  TestMinMax<F8p4>::run(s);
  TestMinMax<F8u>::run(s);
  return s.finalize();
}
