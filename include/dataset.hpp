#ifndef INCLUDE_DATASET_HPP_
#define INCLUDE_DATASET_HPP_
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
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
  std::optional<T> add_subject(std::map<std::string, std::string> const& args);
  template <typename T = Subject>
  std::optional<T> get_subject(int const& idx);
  bool is_subject(int idx);

  std::filesystem::path const participants_filepath;
  std::vector<std::string> participants_properties;
  std::vector<std::map<std::string, std::string>> participants_table;
  Json::Value participants_sidecar;
  std::filesystem::path const participants_sidecar_filepath;

  bool confirm_add_subject_(int subject_idx, std::string subject_name);

 private:
  void load_participants_table_(void);
  std::filesystem::path bids_dir_;
  bool silent_;
};

template <typename T>
std::optional<T> Dataset::add_subject(
    std::map<std::string, std::string> const& args) {
  assert(args.size() <= this->participants_properties.size());

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
    return;
  }

  if (!this->silent_) {
    if (!this->confirm_add_subject_(idx, name)) {
      return this->get_subject<T>(idx);
    }
  }
  T subject(*this, args);
}

template <typename T>
std::optional<T> Dataset::get_subject(int const& idx) {
  std::string participant_id = ensure_participant_id(idx);
  auto it = std::find_if(
      this->participants_table.begin(), this->participants_table.end(),
      [&](auto i) { return i["participant_id"] == participant_id; });

  if (it == this->participants_table.end()) return std::nullopt;
  return std::optional<T>(std::in_place, *this, *it);
}

#endif /* INCLUDE_DATASET_HPP_ */
