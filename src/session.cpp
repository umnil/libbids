#include <pybind11/pybind11.h>

#include "session.hpp"
#include "subject.hpp"

namespace py = pybind11;

// Session::Session(Subject const& subject, int idx)
Session::Session(py::object subject, int idx)
    : Entity("Session", std::nullopt, idx), subject_(subject) {
  std::filesystem::create_directory(this->path());
}

std::filesystem::path Session::path() const {
  std::string path = this->subject_.attr("path").cast<std::string>();
  std::filesystem::path subject_path(path);
  // return this->subject_.path() / this->id();
  return subject_path / this->id();
}

std::string Session::prefix() const {
  std::string subject_id = this->subject_.attr("id").cast<std::string>();
  return subject_id + "_" + this->id();
}
