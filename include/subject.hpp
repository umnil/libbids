#ifndef INCLUDE_SUBJECT_HPP_
#define INCLUDE_SUBJECT_HPP_
#include <any>
#include <map>
#include <string>

#include "dataset.hpp"
class Subject {
 public:
  Subject(Dataset const& dataset, std::map<std::string, std::any> const& args);
  template <typename T = std::any>
  T const& operator[](std::string const& idx);

 private:
  std::string ensure_participant_id_(std::string const& participant_id);
  Dataset const& dataset_;
  std::map<std::string, std::any> properties_;
};

template <typename T>
T const& Subject::operator[](std::string const& idx) {
  T value = this->properties_[idx];
  return value;
}
#endif /* INCLUDE_SUBJECT_HPP_ */
