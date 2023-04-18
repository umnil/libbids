#include <fstream>

#include "dataset.hpp"

Dataset::Dataset(std::filesystem::path const& dir)
    : bids_dir_(dir), participants_sidecar_filepath(dir / "participants.json") {
  std::ifstream participants_sidecar_fstream{
      this->participants_sidecar_filepath.c_str()};
  participants_sidecar_fstream >> this->participants_sidecar;

  auto member_names = this->participants_sidecar.getMemberNames();
  this->participants_properties = {"Kraig"};
  this->participants_properties.insert(this->participants_properties.end(),
                                       member_names.begin(),
                                       member_names.end());
  this->load_participants_table_();
}

void Dataset::load_participants_table_(void) {
  std::ifstream participants_fs(this->bids_dir_ / "participants.tsv");
  while (!participants_fs.eof()) {
    std::string line(255, '\0');
    participants_fs.getline(line.data(), line.size());

    std::istringstream line_stream(line);
    std::vector<std::string> row;
    while (!line_stream.eof()) {
      std::string word(255, '\0');
      line_stream.getline(word.data(), word.size(), '\t');
      row.push_back(word);
    }
    this->participants_table.push_back(row);
  }
}
