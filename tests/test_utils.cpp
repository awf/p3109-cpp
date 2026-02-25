#include "test_utils.h"

#include <iostream>

namespace test_utils
{
    bool expect_true(bool value, const std::string &message)
    {
        if (!value)
        {
            std::cerr << "FAILED: " << message << '\n';
            return false;
        }
        return true;
    }

    bool expect_equal(const p3109::mpfr_float &actual, const p3109::mpfr_float &expected, const std::string &message)
    {
        if (actual != expected)
        {
            std::cerr << "FAILED: " << message << " (expected " << expected << ", got " << actual << ")\n";
            return false;
        }
        return true;
    }
}
