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
      .def_property_readonly("id", &Entity::id);

  py::class_<Session>(m, "Session")
      .def(py::init([](py::object sub, int i) {
        return Session(*py::cast<Subject*>(sub), i);
      }))
      .def_property_readonly("path", &Session::path)
      .def_property_readonly("prefix", &Session::prefix);
}
