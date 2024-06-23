#include <pybind11/pybind11.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

#include "dataset.hpp"
#include "subject.hpp"
#include "utils.hpp"

Dataset::Dataset(const std::filesystem::path& bids_dir, bool silent)
    : bids_dir(bids_dir), silent_(silent) {
  // Load participants sidecar
  std::ifstream sidecar_file(this->participants_sidecar_filepath());
  if (sidecar_file.is_open()) {
    sidecar_file >> this->participants_sidecar_;
    sidecar_file.close();
  } else {
    std::cerr << "Could not open participants sidecar file." << std::endl;
  }

  // Load participant table
  this->load_participants_table_();
}

std::optional<Subject> Dataset::add_subject(
    const std::unordered_map<std::string, std::string>& args) {
  if (args.size() > this->participants_properties().size()) {
    std::cerr << "Too many arguments" << std::endl;
    throw std::runtime_error("AssertionError: Too Many Arguments");
    return std::nullopt;
  }

  if (!this->silent_ &&
      !this->confirm_add_subject_(std::stoi(args.at("participant_id")),
                                  args.at("name"))) {
    return this->get_subject(std::stoi(args.at("participant_id")));
  }

  Subject subject(this->shared_from_this(), args);
  if (std::find(this->get_subjects().begin(), this->get_subjects().end(),
                std::stoi(subject.get_participant_label())) ==
      this->get_subjects().end()) {
    this->append_participant(subject);
  }

  std::filesystem::create_directories(this->bids_dir /
                                      subject.get_participant_id());
  this->load_participants_table_();
  return subject;
}

void Dataset::append_participant(const Subject& subject) {
  std::ofstream participant_file(participants_filepath(), std::ios::app);
  if (participant_file.is_open()) {
    for (const auto& [key, value] : subject.to_dict()) {
      participant_file << key << "\t" << value << "\n";
    }
    participant_file.close();
  } else {
    std::cerr << "Could not open participants file for writing." << std::endl;
  }
}

std::optional<Subject> Dataset::get_subject(int idx) const {
  auto participant_id = Subject::ensure_participant_id(idx);
  auto it = std::find_if(
      this->participants_table_.begin(), this->participants_table_.end(),
      [&participant_id](
          const std::unordered_map<std::string, std::string>& participant) {
        for (auto& [k, v] : participant)
          return participant.at("participant_id") == participant_id;
      });

  if (it != participants_table_.end()) {
    return Subject(this->shared_from_this(), *it);
  } else {
    return std::nullopt;
  }
}

std::vector<int> Dataset::get_subjects() const {
  std::vector<int> subjects;
  for (const auto& participant : participants_table_) {
    subjects.push_back(std::stoi(participant.at("participant_id").substr(4)));
  }
  return subjects;
}

bool Dataset::is_subject(int idx) const { return get_subject(idx).has_value(); }

bool Dataset::confirm_add_subject_(int subject_idx,
                                   const std::string& subject_name) {
  if (!this->is_subject(subject_idx)) {
    std::string prompt = std::string(
        "New Participant ID\nPlease Confirm that this is a new participant.");
    py::object qprompt = py::module_::import("libbids").attr("qprompt");
    return qprompt(prompt, "OK", "No. No this participant is not new")
        .cast<bool>();
  } else {
    auto subject = get_subject(subject_idx);
    if (subject) {
      assert(subject_name == subject->get_participant_name());
    }
    return false;
  }
}

// Properties
std::vector<std::string> Dataset::participants_properties() const {
  std::vector<std::string> properties;
  properties.push_back("participant_id");
  for (const auto& member : this->participants_sidecar_.getMemberNames()) {
    properties.push_back(member);
  }
  return properties;
}

std::filesystem::path Dataset::participants_filepath() const {
  return bids_dir / "participants.tsv";
}

std::unordered_map<std::string, std::string> Dataset::participants_sidecar()
    const {
  std::unordered_map<std::string, std::string> sidecar_map;
  for (const auto& member : participants_sidecar_.getMemberNames()) {
    sidecar_map[member] = participants_sidecar_[member].asString();
  }
  return sidecar_map;
}

std::filesystem::path Dataset::participants_sidecar_filepath() const {
  return bids_dir / "participants.json";
}

std::vector<std::unordered_map<std::string, std::string>>
Dataset::participant_table() const {
  return this->participants_table_;
}

void Dataset::load_participants_table_(void) {
  if (!std::filesystem::exists(this->participants_filepath())) return;

  std::ifstream participants_fs(this->participants_filepath());
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
    for (auto const& i : this->participants_properties()) found |= (i == word);
    assert(found);
    header.push_back(word);
  }

  while (!participants_fs.eof()) {
    std::string line(255, '\0');
    participants_fs.getline(line.data(), line.size());
    line = trim(line);

    std::istringstream line_stream(line);
    std::unordered_map<std::string, std::string> row;
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
    if (row.size() > 1) this->participants_table_.push_back(row);
  }
}
