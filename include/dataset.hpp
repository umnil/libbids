#ifndef INCLUDE_DATASET_HPP_
#define INCLUDE_DATASET_HPP_

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "json/json.h"

class Subject;

/**
 * @brief Represents a dataset with methods for managing participants.
 *
 * This class provides functionality to manage participant data within
 * a specified BIDS dataset directory.
 */
class Dataset : public std::enable_shared_from_this<Dataset> {
 public:
  /**
   * @brief Constructs a Dataset object.
   *
   * Initializes a Dataset object with the provided BIDS directory path.
   *
   * @param bids_dir The path to the BIDS dataset directory.
   * @param silent If true, suppresses interactive prompts.
   */
  Dataset(const std::filesystem::path& bids_dir, bool silent = false);

  /**
   * @brief Adds a new subject/participant to the dataset.
   *
   * Creates and adds a new subject/participant to the dataset.
   *
   * @param args A map containing properties of the subject/participant.
   * @return An optional Subject object representing the added participant,
   *         or std::nullopt if the participant could not be added.
   */
  std::optional<Subject> add_subject(
      const std::unordered_map<std::string, std::string>& args);

  /**
   * @brief Appends participant data to the dataset file.
   *
   * Appends participant data to the dataset file, which stores information
   * about each participant.
   *
   * @param subject The Subject object representing the participant to append.
   */
  void append_participant(const Subject& subject);

  /**
   * @brief Retrieves a subject/participant from the dataset.
   *
   * Retrieves a subject/participant from the dataset based on its index.
   *
   * @param idx The index of the subject/participant to retrieve.
   * @return An optional Subject object representing the retrieved participant,
   *         or std::nullopt if the participant does not exist.
   */
  std::optional<Subject> get_subject(int idx) const;

  /**
   * @brief Retrieves a list of indices of all subjects/participants in the
   * dataset.
   *
   * Retrieves a list of indices representing all subjects/participants
   * currently present in the dataset.
   *
   * @return A vector of integers containing the indices of all
   * subjects/participants.
   */
  std::vector<int> get_subjects() const;

  /**
   * @brief Checks if a subject/participant exists in the dataset.
   *
   * Checks whether a subject/participant with the specified index exists
   * in the dataset.
   *
   * @param idx The index of the subject/participant to check.
   * @return true if the subject/participant exists, otherwise false.
   */
  bool is_subject(int idx) const;

  // Properties
  std::vector<std::string> participants_properties() const;
  std::filesystem::path participants_filepath() const;
  std::unordered_map<std::string, std::string> participants_sidecar() const;
  std::filesystem::path participants_sidecar_filepath() const;
  std::vector<std::unordered_map<std::string, std::string>> participant_table()
      const;
  std::filesystem::path bids_dir;

 private:
  /**
   * @brief Prompts the user to confirm adding a new subject/participant.
   *
   * Displays a prompt dialog to confirm adding a new subject/participant
   * with the specified index and name.
   *
   * @param subject_idx The index of the subject/participant to add.
   * @param subject_name The name of the subject/participant to add.
   * @return true if the user confirms adding the subject/participant, otherwise
   * false.
   */
  bool confirm_add_subject_(int subject_idx, const std::string& subject_name);
  void load_participants_table_(void);

  bool silent_;
  Json::Value participants_sidecar_;
  std::vector<std::unordered_map<std::string, std::string>> participants_table_;
};

#endif  // INCLUDE_DATASET_HPP_
