#pragma once

#include <string>

#include "p3109.h"

namespace p3109::tests
{
    enum class run_status
    {
        pass,
        fail,
        unsupported,
    };

    run_status run_decode(unsigned k, unsigned p, Signedness sigma, const std::string &test_name);
    run_status run_encode(unsigned k, unsigned p, Signedness sigma, const std::string &test_name);
}
