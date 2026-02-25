
#include <array>
#include <iostream>

#include "p3109.h"

int main()
{
    p3109::ensure_mpfr_precision();

    using Binary8p3 = p3109::binary<8, 3, p3109::Signed, p3109::Extended>;

    for (std::uint64_t x : {0, 1, 42, 64, 126, 127, 128, 129, 253, 254, 255})
    {
        std::cout << "Decode(" << x << ") = " << p3109::Decode(Binary8p3{x}) << '\n';
    }

    return 0;
}
