#include <filesystem>
#include <map>

#include "dataset.hpp"
#include "gtest/gtest.h"
#include "subject.hpp"

TEST(Subject, ctor) {
  std::filesystem::path cwd = std::filesystem::current_path();
  Dataset dataset(cwd / "tests" / "bids");

  std::map<std::string, std::any> args = {
      {"name", std::make_any<std::string>("Leeroy Jenkins")}};
  Subject subject(dataset, args);
  EXPECT_STREQ(std::any_cast<std::string>(subject["name"]).data(),
               "Leeroy Jenkins");
}
