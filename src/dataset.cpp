#include <QMessageBox>
#include <cassert>
#include <fstream>
#include <regex>

#include "dataset.hpp"
#include "subject.hpp"

Dataset::Dataset(std::filesystem::path const& dir, bool silent)
    : bids_dir_(dir),
      silent_(silent),
      participants_filepath(dir / "participants.tsv"),
      participants_sidecar_filepath(dir / "participants.json") {
  assert(std::filesystem::exists(this->bids_dir_));
  std::ifstream participants_sidecar_fstream{
      this->participants_sidecar_filepath.c_str()};
  participants_sidecar_fstream >> this->participants_sidecar;

  auto member_names = this->participants_sidecar.getMemberNames();
  this->participants_properties = {"participant_id"};
  this->participants_properties.insert(this->participants_properties.end(),
                                       member_names.begin(),
                                       member_names.end());
  this->load_participants_table_();
  this->args_.push_back("App");
  for (auto s : this->args_) {
    this->argvs_.push_back(s.data());
  }
  this->argc_ = this->args_.size();
  this->app_.emplace(
      std::make_unique<QApplication>(this->argc_, this->argvs_.data()));
}

void Dataset::append_participant(Subject const& subject) {
  this->participants_table.push_back(subject.to_dict());
  this->save_participants_table_();
}

bool Dataset::confirm_add_subject_(int subject_idx, std::string subject_name) {
  if (!this->is_subject(subject_idx)) {
    int mb = QMessageBox::warning(
        nullptr, "Warning",
        "New Participant ID\nPlease confirm that this is a new participant",
        QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::Cancel);
    return mb == QMessageBox::StandardButton::Ok;
  } else {
    return false;
  }
}

std::vector<int> Dataset::get_existing_subjects(void) {
  std::vector<int> subject_ids;
  std::regex subject_regex("sub-(\\d+)");

  for (auto const& entry :
       std::filesystem::directory_iterator(this->bids_dir_)) {
    if (entry.is_directory()) {
      std::smatch match;
      std::string folder_name = entry.path().filename().string();

      if (std::regex_match(folder_name, match, subject_regex)) {
        int subject_id = std::stoi(match[1].str());
        subject_ids.push_back(subject_id);
      }
    }
  }

  return subject_ids;
}

bool Dataset::is_subject(int idx) {
  return this->get_subject<Subject>(idx).has_value();
}

bool Dataset::is_silent(void) const { return this->silent_; }

void Dataset::load_participants_table_(void) {
  if (!std::filesystem::exists(this->participants_filepath)) return;

  std::ifstream participants_fs(this->participants_filepath);
  std::string header_line(255, '\0');
  participants_fs.getline(header_line.data(), header_line.size());
  header_line = trim(header_line);
  std::istringstream header_stream(header_line);
  std::vector<std::string> header;
  while (!header_stream.eof()) {
    std::string word(255, '\0');
    header_stream.getline(word.data(), word.size(), '\t');
    word = trim(word);
    bool found = false;
    for (auto const& i : this->participants_properties) found |= (i == word);
    assert(found);
    header.push_back(word);
  }

  while (!participants_fs.eof()) {
    std::string line(255, '\0');
    participants_fs.getline(line.data(), line.size());
    line = trim(line);

    std::istringstream line_stream(line);
    std::map<std::string, std::string> row;
    int i = 0;
    while (!line_stream.eof()) {
      std::string value(255, '\0');
      line_stream.getline(value.data(), value.size(), '\t');
      value = trim(value);
      if (!value.empty()) {
        std::string& key = header[i];
        row.emplace(std::pair(key, value));
        i++;
      }
    }
    this->participants_table.push_back(row);
  }
}

void Dataset::save_participants_table_(void) {
  std::ofstream participants_fs(this->participants_filepath);

  if (!participants_fs) return;

  // Write header
  if (!this->participants_table.empty()) {
    for (auto const& pair : this->participants_table[0]) {
      participants_fs << pair.first << '\t';
    }
    participants_fs << '\n';
  }

  // Write data rows
  for (auto const& row : this->participants_table) {
    for (auto const& pair : row) {
      participants_fs << pair.second << '\t';
    }
    participants_fs << '\n';
  }

  participants_fs.close();
}
