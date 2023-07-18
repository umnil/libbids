#include <json/json.h>
#include <pybind11/pybind11.h>

#include <QMessageBox>
#include <QPushButton>
#include <fstream>
#include <map>
#include <optional>

#include "dataset.hpp"
#include "session.hpp"
#include "subject.hpp"

// Subject::Subject(Dataset& dataset,
// std::map<std::string, std::string> const& args)
//: Entity("Subject", std::nullopt), dataset_(dataset), properties_(args) {
Subject::Subject(py::object dataset,
                 std::map<std::string, std::string> const& args)
    : Entity("Subject", std::nullopt), dataset_(dataset), properties_(args) {
  this->properties_["participant_id"] =
      this->ensure_participant_id_(this->properties_["participant_id"]);
  this->participant_id_ = this->properties_["participant_id"];
  this->participant_name_ = this->properties_["name"];
  std::filesystem::create_directories(this->path());
  this->set_label_(this->get_participant_label());
}

Session Subject::add_session(bool silent) {
  int n = this->get_n_sessions();
  if (silent || this->confirm_add_session_()) {
    return Session(*this, n + 1);
  } else {
    return get_session(n);
  }
}

std::string const& Subject::get_participant_id() const {
  return this->participant_id_;
}

std::string const& Subject::get_participant_name() const {
  return this->participant_name_;
}

std::string const& Subject::get_participant_label() const {
  return this->participant_id_.substr(4);
}

std::map<std::string, std::string> Subject::get_participant_sidecar() const {
  std::map<std::string, std::string> sidecar;
  std::ifstream file(this->dataset_.participants_sidecar_filepath());
  if (file.is_open()) {
    Json::Value sidecar_json;
    file >> sidecar_json;
    // Convert the JSON Value to a map of strings
    for (auto const& key : sidecar_json.getMemberNames()) {
      sidecar[key] = sidecar_json[key].asString();
    }
  }
  return sidecar;
}

int Subject::get_n_sessions() const {
  if (!std::filesystem::exists(this->path())) return 0;
  int count = 0;
  for (const auto& entry : std::filesystem::directory_iterator(this->path())) {
    if (entry.is_directory() &&
        entry.path().filename().string().find("ses-") == 0) {
      count++;
    }
  }
  return count;
}

Session Subject::get_session(int session_id) {
  assert(session_id <= get_n_sessions());
  return Session(*this, session_id);
}

std::filesystem::path Subject::path() const {
  // return this->dataset_.bids_dir() / this->participant_id_;
  return this->dataset_.attr("bids_dir")().cast<std::filesystem::path>() /
         this->participant_id_;
}

std::map<std::string, std::string> const& Subject::to_dict(void) const {
  return this->properties_;
}

Subject& Subject::operator=(Subject&& other) {
  this->dataset_ = std::move(other.dataset_);
  this->participant_id_ = other.participant_id_;
  this->participant_name_ = other.participant_name_;
  this->properties_ = other.properties_;
  return *this;
}

bool Subject::confirm_add_session_() {
  int n_sessions = get_n_sessions();
  if (n_sessions > 0) {
    QMessageBox msgbox;
    msgbox.setText(QString("OK to start new session: #") +
                   QString::number(n_sessions + 1) + QString("?"));
    QPushButton* ybtn = msgbox.addButton(QString("Ok"), QMessageBox::YesRole);
    QPushButton* nbtn = msgbox.addButton(
        QString("No, use previous session #") + QString::number(n_sessions),
        QMessageBox::NoRole);
    msgbox.exec();
    if (msgbox.clickedButton() == ybtn) {
      return true;
    } else {
      return false;
    }
  } else {
    return true;
  }
}

std::string Subject::ensure_participant_id_(std::string const& id) {
  std::string retval;
  if (std::isdigit(id[0])) {
    int int_id = std::stoi(id);
    retval = "sub-" + std::to_string(int_id);
  } else {
    retval = id;
    if (retval.substr(0, 4) != "sub-") {
      retval = "sub-" + retval;
    }
  }
  return retval;
}
