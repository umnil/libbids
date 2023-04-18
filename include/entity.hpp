#ifndef INCLUDE_ENTITY_HPP_
#define INCLUDE_ENTITY_HPP_

#include <optional>
#include <string>

class Entity {
 public:
  Entity(std::string name, std::optional<std::string> key, int value = 0,
         int padding = 2);
  Entity(std::string name, std::optional<std::string> key, std::string value,
         int padding = 2);

  std::string const& id(void);

 private:
  void set_key_(std::optional<std::string> const& key);

  std::string id_;
  int index_;
  std::string key_;
  std::string label_;
  std::string name_;
  int padding_;
};
#endif /* INCLUDE_ENTITY_HPP_ */
