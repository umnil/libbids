#include <gtest/gtest.h>

#include <optional>
#include <string>

#include "entity.hpp"

TEST(EntityTest, ConstructorWithNameAndOptionalKeyAndIntValue) {
  std::string name = "Entity";
  std::optional<std::string> key = "KEY";
  int value = 42;
  int padding = 2;

  Entity entity(name, key, value, padding);

  EXPECT_EQ(entity.id(), "KEY-42");
}

TEST(EntityTest, ConstructorWithNameAndOptionalKeyAndStringValue) {
  std::string name = "Entity";
  std::optional<std::string> key = "KEY";
  std::string value = "VALUE";
  int padding = 2;

  Entity entity(name, key, value, padding);

  EXPECT_EQ(entity.id(), "KEY-VALUE");
}

TEST(EntityTest, ConstructorWithNameAndOptionalKeyAndInvalidStringValue) {
  std::string name = "Entity";
  std::optional<std::string> key = "KEY";
  std::string value = "INVALID";
  int padding = 2;

  Entity entity(name, key, value, padding);

  EXPECT_EQ(entity.id(), "KEY-INVALID");
}
