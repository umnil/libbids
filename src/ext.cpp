#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>

#include <memory>

#include "add.hpp"
#include "dataset.hpp"
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
      .def(py::init<std::shared_ptr<Dataset const>,
                    std::unordered_map<std::string, std::string>>())
      .def("__getitem__",
           [](Subject& self, std::string const& key) { return self[key]; })
      .def("add_session", &Subject::add_session, py::arg("silent") = false)
      .def_property_readonly("dataset", &Subject::dataset)
      .def("ensure_participant_id",
           static_cast<std::string (*)(std::string const&)>(
               &Subject::ensure_participant_id))
      .def("ensure_participant_id",
           static_cast<std::string (*)(int)>(&Subject::ensure_participant_id))
      .def("get_participant_id", &Subject::get_participant_id)
      .def("get_participant_name", &Subject::get_participant_name)
      .def("get_participant_label", &Subject::get_participant_label)
      .def("get_participant_sidecar", &Subject::get_participant_sidecar)
      .def("get_n_sessions", &Subject::get_n_sessions)
      .def_property_readonly("id", &Subject::id)
      .def_property_readonly("index", &Subject::index)
      .def_property_readonly("label", &Subject::label)
      .def_property_readonly("path", &Subject::path)
      .def_property_readonly("padding", &Subject::padding)
      .def("to_dict", &Subject::to_dict);

  py::class_<Dataset, std::shared_ptr<Dataset>>(m, "Dataset")
      .def(py::init<const std::filesystem::path&, bool>(), py::arg("bids_dir"),
           py::arg("silent") = false)
      .def("add_subject", &Dataset::add_subject)
      .def("append_participant", &Dataset::append_participant,
           py::arg("subject"))
      .def("get_subject", &Dataset::get_subject, py::arg("idx"))
      .def("get_subjects", &Dataset::get_subjects)
      .def("is_subject", &Dataset::is_subject, py::arg("idx"))
      .def_property_readonly("participant_table", &Dataset::participant_table);
}
