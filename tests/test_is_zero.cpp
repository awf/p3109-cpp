#include <string>

#include "test_utils.h"

// IsZero tests: predicate operation exemplar
template <typename Format>
struct TestIsZero {
  static bool test_nan() { return test_utils::expect_true(!p3109::IsZero(Format::nan), "IsZero(NaN) should be false"); }

  static bool test_posinf()
  {
    return test_utils::expect_true(!p3109::IsZero(Format::inf), "IsZero(+inf) should be false");
  }

  static bool test_neginf()
  {
    if constexpr (Format::signedness == p3109::Signedness::Unsigned)
      return true;
    return test_utils::expect_true(!p3109::IsZero(Format::ninf), "IsZero(-inf) should be false");
  }

  static bool test_zero()
  {
    const auto z = p3109::Encode<Format>(p3109::mpfr_float(0.0));
    return test_utils::expect_true(p3109::IsZero(z), "IsZero(0) should be true");
  }

  static bool test_nonzero()
  {
    const auto z = p3109::Encode<Format>(p3109::mpfr_float(1.0));
    return test_utils::expect_true(!p3109::IsZero(z), "IsZero(1) should be false");
  }

  static void run(test_utils::suite &s)
  {
    s.with_path(Format::name(), [&s] {
      s.run({"nan"}, test_nan());
      s.run({"posinf"}, test_posinf());
      s.run({"neginf"}, test_neginf());
      s.run({"zero"}, test_zero());
      s.run({"nonzero"}, test_nonzero());
    });
  }
};

int main()
{
  test_utils::suite s{"is_zero"};

  using Binary8p3se = p3109::binary<8, 3, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using Binary8p4se = p3109::binary<8, 4, p3109::Signedness::Signed, p3109::Domain::Extended>;
  using Binary8p3uf = p3109::binary<8, 3, p3109::Signedness::Unsigned, p3109::Domain::Finite>;

  TestIsZero<Binary8p3se>::run(s);
  TestIsZero<Binary8p4se>::run(s);
  TestIsZero<Binary8p3uf>::run(s);

  return s.finalize();
}
