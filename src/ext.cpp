#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <memory>

#include "add.hpp"
#include "entity.hpp"
#include "session.hpp"
#include "subject.hpp"

namespace py = pybind11;

PYBIND11_MODULE(clibbids, m) {
  m.doc() = "Brain Imaging Data Structure";
  m.def("add", &add);

  // =================================================================
  // Classes
  // =================================================================
  py::class_<Entity>(m, "Entity")
      .def(py::init<std::string, std::optional<std::string>, int, int>(),
           py::arg("name"), py::arg("key") = py::none(), py::arg("value") = 0,
           py::arg("padding") = 2)
      .def(
          py::init<std::string, std::optional<std::string>, std::string, int>(),
          py::arg("name"), py::arg("key") = py::none(), py::arg("value") = "0",
          py::arg("padding") = 2)
      .def_property_readonly("id", &Entity::id)
      .def_property_readonly("index", &Entity::index)
      .def_property_readonly("label", &Entity::label)
      .def_property_readonly("padding", &Entity::padding);

  py::class_<Session>(m, "Session")
      .def(py::init<Subject, int>())
      .def_property_readonly("id", &Session::id)
      .def_property_readonly("index", &Session::index)
      .def_property_readonly("label", &Session::label)
      .def_property_readonly("path", &Session::path)
      .def_property_readonly("padding", &Session::padding)
      .def_property_readonly("prefix", &Session::prefix)
      .def_property_readonly("subject", &Session::subject);

  py::class_<Subject>(m, "Subject")
      .def(py::init<py::object, std::map<std::string, std::string>>())
      .def_property_readonly("dataset", &Subject::dataset)
      .def("ensure_participant_id",
           static_cast<std::string (*)(std::string const&)>(
               &Subject::ensure_participant_id))
      .def("ensure_participant_id",
           static_cast<std::string (*)(int)>(&Subject::ensure_participant_id))
      .def_property_readonly("id", &Subject::id)
      .def_property_readonly("index", &Subject::index)
      .def_property_readonly("label", &Subject::label)
      .def_property_readonly("path", &Subject::path)
      .def_property_readonly("padding", &Subject::padding);
}
