#ifndef INCLUDE_SUBJECT_HPP_
#define INCLUDE_SUBJECT_HPP_
#include <pybind11/pybind11.h>

#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "entity.hpp"

namespace py = pybind11;

// class Dataset;
class Session;

class Subject : public Entity {
 public:
  // Subject(Dataset& dataset, std::map<std::string, std::string> const& args);
  Subject(py::object dataset, std::map<std::string, std::string> const& args);
  Session add_session(bool silent = false);
  py::object dataset(void) const;
  static std::string ensure_participant_id(std::string const& id);
  static std::string ensure_participant_id(int id);
  std::string const& get_participant_id() const;
  std::string const& get_participant_name() const;
  std::string get_participant_label() const;
  std::map<std::string, std::string> get_participant_sidecar() const;
  int get_n_sessions() const;
  Session get_session(int session_id);
  std::filesystem::path path() const;
  std::map<std::string, std::string> const& to_dict(void) const;

  template <typename T = std::string>
  T const& operator[](std::string const& idx) const;

  template <typename T = std::string>
  T& operator[](std::string const& idx);

 private:
  bool confirm_add_session_();
  // Dataset& dataset_;
  py::object dataset_;
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
