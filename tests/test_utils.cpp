#include "test_utils.h"

#include <iostream>
#include <sstream>

namespace test_utils {
  namespace {
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

    std::string join_path(const std::vector<std::string> &path)
    {
      std::ostringstream os;
      bool first = true;
      for (const auto &part : path)
      {
        if (!first)
          os << '/';
        os << part;
        first = false;
      }
      return os.str();
    }
  } // namespace

  suite::suite(std::string suite_name) : suite_name_(std::move(suite_name))
  {
    p3109::ensure_mpfr_precision();
    std::cout << "[==========] Running " << suite_name_ << " tests\n";
  }

  void suite::run(std::initializer_list<std::string_view> path, bool passed)
  {
    const std::string full_path =
      path_prefix_.empty() ? join_path(path) : (join_path(path_prefix_) + "/" + join_path(path));
    const std::string test_name = suite_name_.empty() ? full_path : (suite_name_ + "/" + full_path);
    std::cout << "[ RUN      ] " << test_name << '\n';
    ++total_;
    if (passed)
    {
      std::cout << "[       OK ] " << test_name << '\n';
    }
    else
    {
      ++failed_;
      std::cout << "[  FAILED  ] " << test_name << '\n';
    }
  }

  int suite::finalize()
  {
    std::cout << "[==========] " << total_ << " tests ran. " << (total_ - failed_) << " passed, " << failed_
              << " failed.\n";
    return failed_ == 0 ? 0 : 1;
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

} // namespace test_utils
