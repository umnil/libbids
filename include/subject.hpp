#ifndef INCLUDE_SUBJECT_HPP_
#define INCLUDE_SUBJECT_HPP_
#include <map>
#include <string>

#include "dataset.hpp"
class Subject {
 public:
  Subject(Dataset const& dataset,
          std::map<std::string, std::string> const& args);
  template <typename T = std::string>
  T const& operator[](std::string const& idx);
  std::map<std::string, std::string> const& to_dict(void) const;

 private:
  std::string ensure_participant_id_(std::string const& participant_id);
  Dataset const& dataset_;
  std::map<std::string, std::string> properties_;
};

template <typename T>
T const& Subject::operator[](std::string const& idx) {
  return this->properties_[idx];
}
#endif /* INCLUDE_SUBJECT_HPP_ */
