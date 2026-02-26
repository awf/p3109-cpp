#include <cstdlib>
#include <iostream>
#include <string>

#include "p3109_test_runner.h"

namespace
{
    bool parse_signedness(const std::string &s, p3109::Signedness &out)
    {
        if (s == "signed" || s == "Signed")
        {
            out = p3109::Signed;
            return true;
        }
        if (s == "unsigned" || s == "Unsigned")
        {
            out = p3109::Unsigned;
            return true;
        }
        return false;
    }

    bool parse_uint_arg(const char *arg, unsigned &out)
    {
        try
        {
            const auto value = std::stoul(arg);
            out = static_cast<unsigned>(value);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        std::cerr << "Usage: p3109_tests <test-name> <K> <P> <signed|unsigned>\n";
        return EXIT_FAILURE;
    }

    const std::string test_name = argv[1];
    unsigned k = 0;
    unsigned p = 0;
    p3109::Signedness sigma = p3109::Signed;

    if (!parse_uint_arg(argv[2], k) || !parse_uint_arg(argv[3], p) || !parse_signedness(argv[4], sigma))
    {
        std::cerr << "Invalid arguments. Usage: p3109_tests <test-name> <K> <P> <signed|unsigned>\n";
        return EXIT_FAILURE;
    }

    constexpr unsigned KMin = 3;
    constexpr unsigned KMax = 16;

    if (k < KMin || k > KMax)
    {
        std::cerr << "Unsupported K=" << k << ". Supported range is " << KMin << ".." << KMax << ".\n";
        return EXIT_FAILURE;
    }

    const auto decode = p3109::tests::run_decode(k, p, sigma, test_name);
    if (decode == p3109::tests::run_status::pass)
        return EXIT_SUCCESS;
    if (decode == p3109::tests::run_status::fail)
        return EXIT_FAILURE;

    const auto encode = p3109::tests::run_encode(k, p, sigma, test_name);
    if (encode == p3109::tests::run_status::pass)
        return EXIT_SUCCESS;
    if (encode == p3109::tests::run_status::fail)
        return EXIT_FAILURE;

    const auto project_ops = p3109::tests::run_project_ops(k, p, sigma, test_name);
    if (project_ops == p3109::tests::run_status::pass)
        return EXIT_SUCCESS;
    if (project_ops == p3109::tests::run_status::fail)
        return EXIT_FAILURE;

    const auto sqrt = p3109::tests::run_sqrt(k, p, sigma, test_name);
    if (sqrt == p3109::tests::run_status::pass)
        return EXIT_SUCCESS;
    if (sqrt == p3109::tests::run_status::fail)
        return EXIT_FAILURE;

    std::cerr << "Unknown test case: " << test_name << "\n";
    return EXIT_FAILURE;
}
