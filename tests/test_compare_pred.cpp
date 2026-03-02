#include "test_utils.h"

// Tests for CompareGreater, CompareGreaterEqual, CompareLess, CompareLessEqual, IsOne, IsSignMinus
template <typename Format>
struct TestComparePred {
  static void run(test_utils::suite &s)
  {
    auto nan = Format::nan;
    auto inf = Format::inf;
    auto one = p3109::Encode<Format>(p3109::mpfr_float(1));
    auto two = p3109::Encode<Format>(p3109::mpfr_float(2));
    auto three = p3109::Encode<Format>(p3109::mpfr_float(3));

    s.with_path(std::string(Format::name()) + "/CompareGreater", [&] {
      s.run({"nan_x"}, !p3109::CompareGreater(nan, one));
      s.run({"nan_y"}, !p3109::CompareGreater(one, nan));
      s.run({"inf_inf"}, !p3109::CompareGreater(inf, inf));
      s.run({"inf_fin"}, p3109::CompareGreater(inf, one));
      s.run({"fin_inf"}, !p3109::CompareGreater(one, inf));
      s.run({"3_2"}, p3109::CompareGreater(three, two));
      s.run({"2_3"}, !p3109::CompareGreater(two, three));
    });

    s.with_path(std::string(Format::name()) + "/CompareGreaterEqual", [&] {
      s.run({"nan_x"}, !p3109::CompareGreaterEqual(nan, one));
      s.run({"inf_inf"}, p3109::CompareGreaterEqual(inf, inf));
      s.run({"inf_fin"}, p3109::CompareGreaterEqual(inf, one));
      s.run({"3_3"}, p3109::CompareGreaterEqual(three, three));
      s.run({"2_3"}, !p3109::CompareGreaterEqual(two, three));
    });

    s.with_path(std::string(Format::name()) + "/CompareLess", [&] {
      s.run({"nan_x"}, !p3109::CompareLess(nan, one));
      s.run({"nan_y"}, !p3109::CompareLess(one, nan));
      s.run({"inf_inf"}, !p3109::CompareLess(inf, inf));
      s.run({"fin_inf"}, p3109::CompareLess(one, inf));
      s.run({"2_3"}, p3109::CompareLess(two, three));
      s.run({"3_2"}, !p3109::CompareLess(three, two));
    });

    s.with_path(std::string(Format::name()) + "/CompareLessEqual", [&] {
      s.run({"nan_x"}, !p3109::CompareLessEqual(nan, one));
      s.run({"inf_inf"}, p3109::CompareLessEqual(inf, inf));
      s.run({"fin_inf"}, p3109::CompareLessEqual(one, inf));
      s.run({"3_3"}, p3109::CompareLessEqual(three, three));
      s.run({"3_2"}, !p3109::CompareLessEqual(three, two));
    });

    s.with_path(std::string(Format::name()) + "/IsOne", [&] {
      s.run({"nan"}, !p3109::IsOne(nan));
      s.run({"inf"}, !p3109::IsOne(inf));
      s.run({"one"}, p3109::IsOne(one));
      s.run({"two"}, !p3109::IsOne(two));
    });

    s.with_path(std::string(Format::name()) + "/IsSignMinus", [&] {
      s.run({"nan"}, !p3109::IsSignMinus(nan));
      s.run({"posinf"}, !p3109::IsSignMinus(inf));
      if constexpr (Format::is_signed)
      {
        s.run({"neginf"}, p3109::IsSignMinus(Format::ninf));
        auto neg2 = p3109::Encode<Format>(p3109::mpfr_float(-2));
        s.run({"neg"}, p3109::IsSignMinus(neg2));
      }
      s.run({"pos"}, !p3109::IsSignMinus(one));
    });
  }
};

int main()
{
  test_utils::suite s{"compare_pred"};
  using F8p3 = p3109::binary<8, 3, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using F8p4 = p3109::binary<8, 4, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using F8u = p3109::binary<8, 3, p3109::Signedness::Unsigned, p3109::Domain::Finite>;
  TestComparePred<F8p3>::run(s);
  TestComparePred<F8p4>::run(s);
  TestComparePred<F8u>::run(s);
  return s.finalize();
}
