#ifndef INCLUDE_SESSION_HPP_
#define INCLUDE_SESSION_HPP_

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>

#include <filesystem>
#include <string>

#include "entity.hpp"

namespace py = pybind11;

// class Subject;

class Session : public Entity {
 public:
  /**
   * Initialize a new session for a giben subject
   * @brief Default Constructor
   * @param subject The subject object that will own this session
   * @param idx The session index number or identifier
   */
  // Session(Subject const& subject, int idx);
  Session(py::object subject, int idx);

  std::filesystem::path path(void) const;
  std::string prefix(void) const;

  py::object subject(void) const;

 private:
  // Subject const& subject_;
  py::object subject_;
};

#endif /* INCLUDE_SESSION_HPP_ */
