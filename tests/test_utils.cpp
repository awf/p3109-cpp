#include "test_utils.h"

#include <iostream>
#include <sstream>

namespace test_utils
{
    namespace
    {
        std::string g_suite;
        int g_total = 0;
        int g_failed = 0;

        std::string join_path(std::initializer_list<std::string_view> path)
        {
            std::ostringstream os;
            bool first = true;
            for (const auto part : path)
            {
                if (!first)
                    os << '/';
                os << part;
                first = false;
            }
            return os.str();
        }
    }

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

    void init(const std::string &suite_name)
    {
        p3109::ensure_mpfr_precision();
        g_suite = suite_name;
        g_total = 0;
        g_failed = 0;
        std::cout << "[==========] Running " << g_suite << " tests\n";
    }

    void run_test(std::initializer_list<std::string_view> path, bool passed)
    {
        const std::string test_name = g_suite.empty() ? join_path(path) : (g_suite + "/" + join_path(path));
        std::cout << "[ RUN      ] " << test_name << '\n';
        ++g_total;
        if (passed)
        {
            std::cout << "[       OK ] " << test_name << '\n';
        }
        else
        {
            ++g_failed;
            std::cout << "[  FAILED  ] " << test_name << '\n';
        }
    }

    int finalize()
    {
        std::cout << "[==========] " << g_total << " tests ran. "
                  << (g_total - g_failed) << " passed, " << g_failed << " failed.\n";
        return g_failed == 0 ? 0 : 1;
    }
}
