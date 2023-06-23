#ifndef INCLUDE_SUBJECT_HPP_
#define INCLUDE_SUBJECT_HPP_
#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "entity.hpp"

class Dataset;
class Session;

class Subject : public Entity {
 public:
  Subject(Dataset const& dataset,
          std::map<std::string, std::string> const& args);
  std::string const& get_participant_id() const;
  std::string const& get_participant_name() const;
  std::filesystem::path path() const;
  Session add_session();
  Session get_session(int session_id);
  int get_n_sessions() const;
  std::map<std::string, std::string> const& to_dict(void) const;
  bool confirm_add_session();
  std::map<std::string, std::string> get_participant_sidecar() const;

  template <typename T = std::string>
  T const& operator[](std::string const& idx) const;

  template <typename T = std::string>
  T& operator[](std::string const& idx);

 private:
  static std::string ensure_participant_id_(std::string const& id);
  Dataset const& dataset_;
  std::string participant_id_;
  std::string participant_name_;
  std::map<std::string, std::string> properties_;
};

template <typename T>
T const& Subject::operator[](std::string const& idx) const {
  return this->properties_.at(idx);
}

template <typename T>
T& Subject::operator[](std::string const& idx) {
  return this->properties_[idx];
}
#endif /* INCLUDE_SUBJECT_HPP_ */
