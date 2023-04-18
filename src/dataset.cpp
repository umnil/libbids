#include <cassert>
#include <fstream>

#include "dataset.hpp"

Dataset::Dataset(std::filesystem::path const& dir)
    : bids_dir_(dir),
      participants_filepath(dir / "participants.tsv"),
      participants_sidecar_filepath(dir / "participants.json") {
  std::ifstream participants_sidecar_fstream{
      this->participants_sidecar_filepath.c_str()};
  participants_sidecar_fstream >> this->participants_sidecar;

  auto member_names = this->participants_sidecar.getMemberNames();
  this->participants_properties = {"participant_id"};
  this->participants_properties.insert(this->participants_properties.end(),
                                       member_names.begin(),
                                       member_names.end());
  this->load_participants_table_();
}

std::optional<Subject> Dataset::add_subject(
    std::map<std::string, std::string> const& args) {
  assert(args.size() <= this->participants_properties.size());
}

void Dataset::load_participants_table_(void) {
  if (!std::filesystem::exists(this->participants_filepath)) return;

  std::ifstream participants_fs(this->participants_filepath);
  std::string header_line(255, '\0');
  participants_fs.getline(header_line.data(), header_line.size());
  std::istringstream header_stream(header_line);
  std::vector<std::string> header;
  while (!header_stream.eof()) {
    std::string word(255, '\0');
    header_stream.getline(word.data(), word.size(), '\t');
    bool found = false;
    for (auto i : this->participants_properties) found |= i == word;
    assert(found);
    header.push_back(word);
  }

  while (!participants_fs.eof()) {
    std::string line(255, '\0');
    participants_fs.getline(line.data(), line.size());

    std::istringstream line_stream(line);
    std::map<std::string, std::string> row;
    int i = 0;
    while (!line_stream.eof()) {
      std::string value(255, '\0');
      line_stream.getline(value.data(), value.size(), '\t');
      std::string& key = header[i];
      row.emplace(std::pair(key, value));
      i++;
    }
    this->participants_table.push_back(row);
  }
}
