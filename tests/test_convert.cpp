#include "test_utils.h"

#include <cmath>
#include <limits>

// Tests for Convert, ConvertFromIEEE754, ConvertToIEEE754
template <typename Format, typename ProjSpec = p3109::ProjectionSpec<p3109::NearestTiesToEven, p3109::SatFinite>>
struct TestConvert {
  static void run(test_utils::suite &s)
  {
    auto nan = Format::nan;
    auto inf = Format::inf;
    auto zero = p3109::Encode<Format>(p3109::mpfr_float(0));
    auto one = p3109::Encode<Format>(p3109::mpfr_float(1));
    auto two = p3109::Encode<Format>(p3109::mpfr_float(2));
    ProjSpec ps{};

    // --- Convert (P3109 -> same P3109 format) ---
    s.with_path(std::string(Format::name()) + "/Convert/SameFormat", [&] {
      s.run({"nan"}, p3109::Convert<Format>(nan, ps).isnan());
      if constexpr (Format::is_extended)
        s.run({"inf_satfinite"}, p3109::Convert<Format>(inf, ps).codepoint == p3109::MaxFiniteOf<Format>().codepoint);
      s.run({"zero"}, p3109::Convert<Format>(zero, ps).codepoint == zero.codepoint);
      s.run({"one"}, p3109::Convert<Format>(one, ps).codepoint == one.codepoint);
      s.run({"two"}, p3109::Convert<Format>(two, ps).codepoint == two.codepoint);

      if constexpr (Format::is_extended)
      {
        p3109::ProjectionSpec<p3109::NearestTiesToEven, p3109::OvfInf> ps_ovf{};
        s.run({"inf_ovfinf"}, p3109::Convert<Format>(inf, ps_ovf).isposinf());
      }
    });

    // --- ConvertFromIEEE754 ---
    s.with_path(std::string(Format::name()) + "/ConvertFromIEEE754", [&] {
      // NaN
      s.run({"nan_float"}, p3109::ConvertFromIEEE754<Format>(std::numeric_limits<float>::quiet_NaN(), ps).isnan());
      s.run({"nan_double"}, p3109::ConvertFromIEEE754<Format>(std::numeric_limits<double>::quiet_NaN(), ps).isnan());

      // Zero (including negative zero -> 0)
      s.run({"zero_float"}, p3109::ConvertFromIEEE754<Format>(0.0f, ps).codepoint == zero.codepoint);
      s.run({"neg_zero_float"}, p3109::ConvertFromIEEE754<Format>(-0.0f, ps).codepoint == zero.codepoint);

      // Exact values
      s.run({"one_float"}, p3109::ConvertFromIEEE754<Format>(1.0f, ps).codepoint == one.codepoint);
      s.run({"two_double"}, p3109::ConvertFromIEEE754<Format>(2.0, ps).codepoint == two.codepoint);

      // Infinity
      if constexpr (Format::is_extended)
      {
        // With SatFinite: IEEE inf -> P3109 MaxFinite (not inf)
        auto max_fin = p3109::MaxFiniteOf<Format>();
        s.run({"inf_float_satfinite"},
          p3109::ConvertFromIEEE754<Format>(std::numeric_limits<float>::infinity(), ps).codepoint == max_fin.codepoint);
        if constexpr (Format::is_signed)
        {
          s.run({"ninf_float_satfinite"},
            p3109::ConvertFromIEEE754<Format>(-std::numeric_limits<float>::infinity(), ps).codepoint ==
              p3109::Encode<Format>(p3109::Decode(max_fin) * -1).codepoint);
        }

        // With OvfInf: IEEE inf -> P3109 inf
        p3109::ProjectionSpec<p3109::NearestTiesToEven, p3109::OvfInf> ps_ovf{};
        s.run({"inf_float_ovfinf"},
          p3109::ConvertFromIEEE754<Format>(std::numeric_limits<float>::infinity(), ps_ovf).isposinf());
        if constexpr (Format::is_signed)
          s.run({"ninf_float_ovfinf"},
            p3109::ConvertFromIEEE754<Format>(-std::numeric_limits<float>::infinity(), ps_ovf).isneginf());
      }

      // Large value saturates with SatFinite
      if constexpr (Format::is_extended)
      {
        auto max_fin = p3109::MaxFiniteOf<Format>();
        s.run({"large_saturates"}, p3109::ConvertFromIEEE754<Format>(1.0e10f, ps).codepoint == max_fin.codepoint);
      }
    });

    // --- ConvertToIEEE754 ---
    s.with_path(std::string(Format::name()) + "/ConvertToIEEE754", [&] {
      // NaN
      s.run({"nan_float"}, std::isnan(p3109::ConvertToIEEE754<float>(nan, ps)));
      s.run({"nan_double"}, std::isnan(p3109::ConvertToIEEE754<double>(nan, ps)));

      // Zero
      s.run({"zero_float"}, p3109::ConvertToIEEE754<float>(zero, ps) == 0.0f);
      s.run({"zero_double"}, p3109::ConvertToIEEE754<double>(zero, ps) == 0.0);

      // Exact values
      s.run({"one_float"}, p3109::ConvertToIEEE754<float>(one, ps) == 1.0f);
      s.run({"two_double"}, p3109::ConvertToIEEE754<double>(two, ps) == 2.0);

      // Infinity (SatFinite: P3109 +inf -> IEEE maxFinite? No -- Saturate with IEEE maxFinite)
      if constexpr (Format::is_extended)
      {
        // P3109 +inf -> Decode = +inf -> RoundToPrecision(+inf) = +inf ->
        // Saturate<SatFinite>(+inf, FLT_MAX) = FLT_MAX
        float inf_as_float = p3109::ConvertToIEEE754<float>(inf, ps);
        s.run({"inf_satfinite_float"}, inf_as_float == std::numeric_limits<float>::max());

        // With OvfInf: P3109 +inf -> float +inf
        p3109::ProjectionSpec<p3109::NearestTiesToEven, p3109::OvfInf> ps_ovf{};
        s.run({"inf_ovfinf_float"}, std::isinf(p3109::ConvertToIEEE754<float>(inf, ps_ovf)));
      }

      // Round-trip: float -> P3109 -> float preserves exact values
      s.run({"roundtrip_1"}, p3109::ConvertToIEEE754<float>(p3109::ConvertFromIEEE754<Format>(1.0f, ps), ps) == 1.0f);
      s.run({"roundtrip_0"}, p3109::ConvertToIEEE754<float>(p3109::ConvertFromIEEE754<Format>(0.0f, ps), ps) == 0.0f);
    });
  }
};

// Test cross-format Convert (P3109 -> different P3109 format)
struct TestConvertCrossFormat {
  static void run(test_utils::suite &s)
  {
    using F8p3 = p3109::binary<8, 3, p3109::Signedness::Signed, p3109::Domain::Extended>;
    using F8p4 = p3109::binary<8, 4, p3109::Signedness::Signed, p3109::Domain::Extended>;
    p3109::ProjectionSpec<p3109::NearestTiesToEven, p3109::SatFinite> ps{};

    s.with_path("CrossFormat/F8p3_to_F8p4", [&] {
      auto nan3 = F8p3::nan;
      auto one3 = p3109::Encode<F8p3>(p3109::mpfr_float(1));
      auto zero3 = p3109::Encode<F8p3>(p3109::mpfr_float(0));
      auto inf3 = F8p3::inf;

      auto one4 = p3109::Encode<F8p4>(p3109::mpfr_float(1));
      auto zero4 = p3109::Encode<F8p4>(p3109::mpfr_float(0));

      s.run({"nan"}, p3109::Convert<F8p4>(nan3, ps).isnan());
      s.run({"inf_satfinite"}, p3109::Convert<F8p4>(inf3, ps).codepoint == p3109::MaxFiniteOf<F8p4>().codepoint);
      s.run({"zero"}, p3109::Convert<F8p4>(zero3, ps).codepoint == zero4.codepoint);
      s.run({"one"}, p3109::Convert<F8p4>(one3, ps).codepoint == one4.codepoint);

      p3109::ProjectionSpec<p3109::NearestTiesToEven, p3109::OvfInf> ps_ovf{};
      s.run({"inf_ovfinf"}, p3109::Convert<F8p4>(inf3, ps_ovf).isposinf());
    });

    s.with_path("CrossFormat/F8p4_to_F8p3", [&] {
      auto nan4 = F8p4::nan;
      auto one4 = p3109::Encode<F8p4>(p3109::mpfr_float(1));
      auto zero4 = p3109::Encode<F8p4>(p3109::mpfr_float(0));
      auto inf4 = F8p4::inf;

      auto one3 = p3109::Encode<F8p3>(p3109::mpfr_float(1));
      auto zero3 = p3109::Encode<F8p3>(p3109::mpfr_float(0));

      s.run({"nan"}, p3109::Convert<F8p3>(nan4, ps).isnan());
      s.run({"inf_satfinite"}, p3109::Convert<F8p3>(inf4, ps).codepoint == p3109::MaxFiniteOf<F8p3>().codepoint);
      s.run({"zero"}, p3109::Convert<F8p3>(zero4, ps).codepoint == zero3.codepoint);
      s.run({"one"}, p3109::Convert<F8p3>(one4, ps).codepoint == one3.codepoint);

      p3109::ProjectionSpec<p3109::NearestTiesToEven, p3109::OvfInf> ps_ovf{};
      s.run({"inf_ovfinf"}, p3109::Convert<F8p3>(inf4, ps_ovf).isposinf());
    });
  }
};

int main()
{
  test_utils::suite s{"convert"};
  using F8p3 = p3109::binary<8, 3, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using F8p4 = p3109::binary<8, 4, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using F8u = p3109::binary<8, 3, p3109::Signedness::Unsigned, p3109::Domain::Finite>;
  TestConvert<F8p3>::run(s);
  TestConvert<F8p4>::run(s);
  TestConvert<F8u>::run(s);
  TestConvertCrossFormat::run(s);
  return s.finalize();
}
