#ifndef INCLUDE_DATASET_HPP_
#define INCLUDE_DATASET_HPP_
#include <QApplication>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "json/json.h"
#include "utils.hpp"
class Subject;
class Dataset {
 public:
  Dataset(std::filesystem::path const& dir, bool silent = false);
  template <typename T = Subject>
  std::unique_ptr<T> add_subject(
      std::map<std::string, std::string> const& args);
  void append_participant(Subject const& subject);
  std::vector<int> get_existing_subjects(void);
  template <typename T = Subject>
  std::unique_ptr<T> get_subject(int const& idx);
  bool is_subject(int idx);
  bool is_silent(void) const;

  std::filesystem::path const bids_dir(void) const;
  std::filesystem::path const participants_filepath(void) const;
  std::filesystem::path const participants_sidecar_filepath(void) const;
  std::vector<std::string> participants_properties;
  std::vector<std::map<std::string, std::string>> participants_table;
  Json::Value participants_sidecar;

  bool confirm_add_subject_(int subject_idx, std::string subject_name);

 private:
  std::filesystem::path participants_filepath_;
  std::filesystem::path participants_sidecar_filepath_;
  std::vector<std::string> args_;
  std::vector<char*> argvs_;
  int argc_;
  std::unique_ptr<QApplication> app_;
  void load_participants_table_(void);
  void save_participants_table_(void);
  std::filesystem::path bids_dir_;
  bool silent_;
};

template <typename T>
std::unique_ptr<T> Dataset::add_subject(
    std::map<std::string, std::string> const& args) {
  assert(args.size() <= this->participants_properties.size() &&
         "Too many arguments");

  std::string const& participant_id = args.at("participant_id");
  std::string const& id = (participant_id.starts_with("sub-"))
                              ? participant_id.substr(4)
                              : participant_id;
  std::string const& name = args.at("name");
  int idx;
  try {
    idx = std::stoi(id);
  } catch (std::exception const& e) {
    std::cout << "Could not interpret participant id of: " << participant_id
              << std::endl;
    return nullptr;
  }

  if (!this->silent_) {
    if (!this->confirm_add_subject_(idx, name)) {
      return this->get_subject<T>(idx);
    }
  }
  std::unique_ptr<T> subject = std::make_unique<T>(*this, args);

  std::vector<int> subject_ids = this->get_existing_subjects();
  if (std::find(subject_ids.begin(), subject_ids.end(), idx) ==
      subject_ids.end()) {
    this->append_participant(*subject);
  }

  std::filesystem::create_directories(this->bids_dir_ /
                                      (*subject)["participant_id"]);
  return subject;
}

template <typename T>
std::unique_ptr<T> Dataset::get_subject(int const& idx) {
  std::string participant_id = ensure_participant_id(idx);
  auto it = std::find_if(
      this->participants_table.begin(), this->participants_table.end(),
      [&](auto i) { return i["participant_id"] == participant_id; });

  if (it == this->participants_table.end()) return nullptr;
  return std::make_unique<T>(*this, *it);
}

#endif /* INCLUDE_DATASET_HPP_ */
