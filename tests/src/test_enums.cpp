#include "enums.hpp"
#include "gtest/gtest.h"

TEST(Modality, is_primary) {
  Modality m = Modality::EEG;
  EXPECT_TRUE(is_primary(m));

  m = Modality::PHYSIO;
  EXPECT_FALSE(is_primary(m));
}
