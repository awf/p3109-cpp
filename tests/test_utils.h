#pragma once

#include <initializer_list>
#include <string>
#include <string_view>

#include "p3109.h"

namespace test_utils
{
    bool expect_true(bool value, const std::string &message);

    bool expect_equal(const p3109::mpfr_float &actual, const p3109::mpfr_float &expected, const std::string &message);

    void init(const std::string &suite_name);
    void run_test(std::initializer_list<std::string_view> path, bool passed);
    int finalize();
}
