#include <algorithm>
#include <optional>
#include <string>

#include "entity.hpp"

Entity::Entity(std::string name, std::optional<std::string> key, int value,
               int padding)
    : index_(value), name_(name), padding_(padding) {
  this->set_key_(key);
  this->label_ =
      std::string(
          this->padding_ -
              std::min(this->padding_,
                       static_cast<int>(std::to_string(this->index_).size())),
          '0') +
      std::to_string(this->index_);
  this->id_ = this->key_ + "-" + this->label_;
}

Entity::Entity(std::string name, std::optional<std::string> key,
               std::string value, int padding)
    : label_(value), name_(name), padding_(padding) {
  this->set_key_(key);
  try {
    this->index_ = std::stoi(value);
  } catch (std::invalid_argument const& ex) {
    this->index_ = -1;
  }
  this->id_ = this->key_ + "-" + this->label_;
}

std::string const& Entity::id(void) { return this->id_; }

void Entity::set_key_(std::optional<std::string> const& key) {
  if (key.has_value())
    this->key_ = key.value();
  else {
    this->key_.assign(this->name_.begin(), this->name_.begin() + 3);
    std::transform(this->key_.begin(), this->key_.end(), this->key_.begin(),
                   [](unsigned char c) { return std::tolower(c); });
  }
}
