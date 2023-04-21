#ifndef INCLUDE_DATASET_HPP_
#define INCLUDE_DATASET_HPP_
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "json/json.h"
class Subject;
class Dataset {
 public:
  Dataset(std::filesystem::path const& dir, bool silent = false);
  template <typename T = Subject>
  std::optional<T> add_subject(std::map<std::string, std::string> const& args);

  std::filesystem::path const participants_filepath;
  std::vector<std::string> participants_properties;
  std::vector<std::map<std::string, std::string>> participants_table;
  Json::Value participants_sidecar;
  std::filesystem::path const participants_sidecar_filepath;

 private:
  bool confirm_add_subject_(int subject_idx, std::string subject_name);
  bool is_subject_(int idx);
  void load_participants_table_(void);
  std::filesystem::path bids_dir_;
  bool silent_;
};

template <typename T>
std::optional<T> Dataset::add_subject(
    std::map<std::string, std::string> const& args) {
  assert(args.size() <= this->participants_properties.size());

  if (!this->silent_) {
    // this->confirm_add_subject_();
  }
}

#endif /* INCLUDE_DATASET_HPP_ */
