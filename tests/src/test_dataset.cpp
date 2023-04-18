#include "dataset.hpp"
#include "gtest/gtest.h"

TEST(Dataset, ctor) {
  std::filesystem::path cwd = std::filesystem::current_path();
  Dataset dataset(cwd / "tests" / "bids");
  EXPECT_STREQ((cwd / "tests" / "bids" / "participants.json").c_str(),
               dataset.participants_sidecar_filepath.c_str());
  EXPECT_EQ(dataset.participants_sidecar["name"].isMember("Description"), true);
  EXPECT_STREQ(dataset.participants_sidecar["name"]["Description"].asCString(),
               "The participant's name");
  bool has_nil = false;
  for (auto i : dataset.participants_properties) has_nil |= i == "nil";
  EXPECT_EQ(has_nil, true);

  EXPECT_EQ(dataset.participants_table.size(), 0);
}
