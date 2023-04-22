#include <QMessageBox>
#include <cassert>
#include <fstream>

#include "dataset.hpp"
#include "subject.hpp"

Dataset::Dataset(std::filesystem::path const& dir, bool silent)
    : bids_dir_(dir),
      silent_(silent),
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
  char** argv = {{'\0'}};
  this->app_.emplace(1, argv);
}

bool Dataset::confirm_add_subject_(int subject_idx, std::string subject_name) {
  if (!this->is_subject(subject_idx)) {
    int mb = QMessageBox::warning(
        nullptr, "Warning",
        "New Participant ID\nPlease confirm that this is a new participant",
        QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::Cancel);
  } else {
    return false;
  }
}
bool Dataset::is_subject(int idx) {
  return this->get_subject<Subject>(idx).has_value();
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
