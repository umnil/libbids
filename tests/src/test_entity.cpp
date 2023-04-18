#include "entity.hpp"
#include "gtest/gtest.h"

TEST(Entity, int_constructor) {
  Entity e("Subject", std::nullopt, 1);
  EXPECT_STREQ("sub-01", e.id().data());
}
