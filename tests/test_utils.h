#pragma once

#include <initializer_list>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "p3109.h"

namespace test_utils {
  class suite {
  public:
    explicit suite(std::string suite_name);

    template <typename Fn>
    void with_path(std::string_view segment, Fn &&fn)
    {
      path_prefix_.emplace_back(segment);
      try
      {
        std::forward<Fn>(fn)();
      }
      catch (...)
      {
        path_prefix_.pop_back();
        throw;
      }
      path_prefix_.pop_back();
    }

    void run(std::initializer_list<std::string_view> path, bool passed);
    int finalize();

  private:
    std::string suite_name_;
    std::vector<std::string> path_prefix_;
    int total_ = 0;
    int failed_ = 0;
  };

  bool expect_true(bool value, const std::string &message);

  bool expect_equal(const p3109::mpfr_float &actual, const p3109::mpfr_float &expected, const std::string &message);
} // namespace test_utils
