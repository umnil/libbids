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

// Test the trim function
TEST(TrimTest, TrimWhitespace) {
  // Test case 1: Trim whitespace from both ends
  std::string str1 = "   Hello, World!   ";
  std::string expected1 = "Hello, World!";
  EXPECT_EQ(trim(str1), expected1);

  // Test case 2: Empty string should remain empty
  std::string str2 = "";
  std::string expected2 = "";
  EXPECT_EQ(trim(str2), expected2);

  // Test case 3: No whitespace to trim
  std::string str3 = "Hello, World!";
  std::string expected3 = "Hello, World!";
  EXPECT_EQ(trim(str3), expected3);

  // Test case 4: Only whitespace characters
  std::string str4 = "    \t\n\r\f\v    ";
  std::string expected4 = "";
  EXPECT_EQ(trim(str4), expected4);
}
