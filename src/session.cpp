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
  std::filesystem::path subject_path =
      this->subject_.attr("path").cast<std::filesystem::path>();
  return subject_path / this->id();
}

std::string Session::prefix() const {
  return this->subject_.attr("id").cast<std::string>() + "_" + this->id();
}

py::object Session::subject(void) const { return this->subject_; }
