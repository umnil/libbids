#include <filesystem>
#include <map>

#include "dataset.hpp"
#include "gtest/gtest.h"
#include "subject.hpp"

TEST(Subject, ctor) {
  std::filesystem::path cwd = std::filesystem::current_path();
  Dataset dataset(cwd / "tests" / "bids");

  std::map<std::string, std::string> args = {{"name", "Leeroy Jenkins"}};
  Subject subject(dataset, args);
  EXPECT_STREQ(subject["name"].data(), "Leeroy Jenkins");
}
