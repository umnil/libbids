#include "gtest/gtest.h"
#include "utils.hpp"

TEST(Utils, ensure_participant_id) {
  // string
  std::string input = "sub-01";
  std::string expected = "sub-01";
  std::string observed = ensure_participant_id(input);
  EXPECT_STREQ(expected.c_str(), observed.c_str());

  input = "02";
  expected = "sub-02";
  observed = ensure_participant_id(input);
  EXPECT_STREQ(expected.c_str(), observed.c_str());

  // integer
  int iinput = 3;
  expected = "sub-03";
  observed = ensure_participant_id(iinput);
  EXPECT_STREQ(expected.c_str(), observed.c_str());
}
