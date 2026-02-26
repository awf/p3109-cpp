#pragma once

#include <initializer_list>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "p3109.h"

namespace test_utils
{
class suite
{
  public:
    class path_guard
    {
      public:
        path_guard() = default;
        path_guard(suite &owner, std::vector<std::string> previous_path);
        ~path_guard();

        path_guard(const path_guard &) = delete;
        path_guard &operator=(const path_guard &) = delete;

        path_guard(path_guard &&other) noexcept;
        path_guard &operator=(path_guard &&other) noexcept;

      private:
        void restore();

        suite *owner_ = nullptr;
        std::vector<std::string> previous_path_;
    };

    explicit suite(std::string suite_name);
    [[nodiscard]] path_guard add_path(std::string_view segment);
    template <typename Fn> void with_path(std::string_view segment, Fn &&fn)
    {
        auto path = add_path(segment);
        std::forward<Fn>(fn)();
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
