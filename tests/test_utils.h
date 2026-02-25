#pragma once

#include <string>

#include "p3109.h"

namespace test_utils
{
    bool expect_true(bool value, const std::string &message);

    bool expect_equal(const p3109::mpfr_float &actual, const p3109::mpfr_float &expected, const std::string &message);
}
