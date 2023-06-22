#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <optional>
#include <vector>

#include "dataset.hpp"
#include "subject.hpp"

class DatasetTest : public ::testing::Test {
 protected:
  std::filesystem::path test_dir;
  std::filesystem::path participants_file;
  std::filesystem::path participants_sidecar_file;
  std::unique_ptr<Dataset> dataset;

  void SetUp() override {
    // Create a temporary test directory
    test_dir = std::filesystem::temp_directory_path() / "dataset_test";
    std::filesystem::create_directory(test_dir);

    // Set up file paths
    participants_file = test_dir / "participants.tsv";
    participants_sidecar_file = test_dir / "participants.json";
    std::filesystem::create_directories(test_dir / "sub-01");
    std::filesystem::create_directories(test_dir / "sub-02");

    // Create participants table file
    std::ofstream participants_table(participants_file);
    participants_table << "participant_id\tname\n";
    participants_table << "sub-01\tJohn\n";
    participants_table << "sub-02\tAlice\n";
    participants_table.close();

    // Create participants sidecar file
    std::ofstream participants_sidecar(participants_sidecar_file);
    participants_sidecar << "{\n";
    participants_sidecar << "\t\"name\": {\n";
    participants_sidecar << "\t\t\"Description\": \"Name of participant\"\n";
    participants_sidecar << "\t}\n";
    participants_sidecar << "}\n";
    participants_sidecar.close();

    // Create the Dataset object
    dataset = std::make_unique<Dataset>(test_dir, true);
  }

  void TearDown() override {
    // Remove the temporary test directory and files
    std::filesystem::remove_all(test_dir);
  }
};

TEST_F(DatasetTest, AddSubject_ValidArguments_ReturnsSubject) {
  std::map<std::string, std::string> args = {{"participant_id", "sub-03"},
                                             {"name", "Bob"}};

  std::optional<Subject> subject = dataset->add_subject(args);

  EXPECT_TRUE(subject.has_value());
  EXPECT_EQ((*subject)["participant_id"], "sub-03");
  EXPECT_EQ((*subject)["name"], "Bob");
}

TEST_F(DatasetTest, AddSubject_TooManyArguments_ReturnsNullopt) {
  std::map<std::string, std::string> args = {
      {"participant_id", "sub-03"},
      {"name", "Bob"},
      {"age", "25"}  // Extra argument
  };

  EXPECT_DEATH({ std::optional<Subject> subject = dataset->add_subject(args); },
               "Assertion failed");
}

TEST_F(DatasetTest, GetSubject_ExistingSubject_ReturnsSubject) {
  std::optional<Subject> subject = dataset->get_subject<Subject>(1);

  EXPECT_TRUE(subject.has_value());
  EXPECT_EQ((*subject)["participant_id"], "sub-01");
  EXPECT_EQ((*subject)["name"], "John");
}

TEST_F(DatasetTest, GetSubject_NonexistentSubject_ReturnsNullopt) {
  std::optional<Subject> subject = dataset->get_subject<Subject>(3);

  EXPECT_FALSE(subject.has_value());
}

TEST_F(DatasetTest, IsSubject_ExistingSubject_ReturnsTrue) {
  bool result = dataset->is_subject(2);

  EXPECT_TRUE(result);
}

TEST_F(DatasetTest, IsSubject_NonexistentSubject_ReturnsFalse) {
  bool result = dataset->is_subject(3);

  EXPECT_FALSE(result);
}

TEST_F(DatasetTest, GetExistingSubjects_ReturnsSubjectIds) {
  std::vector<int> subject_ids = dataset->get_existing_subjects();

  EXPECT_EQ(subject_ids.size(), 2);
  EXPECT_EQ(subject_ids[0], 1);
  EXPECT_EQ(subject_ids[1], 2);
}
