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

  std::string const& id(void) const;
  int const index(void) const;
  std::string const& label(void) const;
  int const padding(void) const;

 protected:
  void set_key_(std::optional<std::string> const& key);
  void set_label_(int value);
  void set_label_(std::string const& value);

 private:
  std::string id_;     // ex. sub-001
  int index_;          // ex. 1
  std::string key_;    // ex. sub
  std::string label_;  // ex. 001
  std::string name_;   // ex. Subject
  int padding_;        // ex. 3
};
#endif /* INCLUDE_ENTITY_HPP_ */
