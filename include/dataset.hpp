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
  Dataset(std::filesystem::path const& dir);
  std::optional<Subject> add_subject(
      std::map<std::string, std::string> const& args);

  std::filesystem::path const participants_filepath;
  std::vector<std::string> participants_properties;
  std::vector<std::map<std::string, std::string>> participants_table;
  Json::Value participants_sidecar;
  std::filesystem::path const participants_sidecar_filepath;

 private:
  void load_participants_table_(void);
  std::filesystem::path bids_dir_;
};
#endif /* INCLUDE_DATASET_HPP_ */
