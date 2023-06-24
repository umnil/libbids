#include <filesystem>
#include <fstream>
#include <memory>

#include "dataset.hpp"
#include "gtest/gtest.h"
#include "session.hpp"
#include "subject.hpp"

// Test fixture for Session class
class SessionTest : public ::testing::Test {
 protected:
  // Set up the test fixture
  void SetUp() override {
    // Create a temporary test directory
    this->test_dir = std::filesystem::temp_directory_path() / "dataset_test";
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

    // Create the Dataset object)
    this->dataset = std::make_unique<Dataset>(this->test_dir, true);
    this->subject = this->dataset->get_subject(1);
  }

  // Tear down the test fixture
  void TearDown() override {
    // Remove the temporary test directory and files
    std::filesystem::remove_all(test_dir);
  }
  std::filesystem::path test_dir;
  std::filesystem::path participants_file;
  std::filesystem::path participants_sidecar_file;
  std::unique_ptr<Dataset> dataset;
  std::unique_ptr<Subject> subject;
};

// Test the path() method of Session
TEST_F(SessionTest, PathTest) {
  // Create a Session object with index 1
  Session session(*subject, 1);

  // Expected path: subject path + session id
  std::filesystem::path expectedPath = subject->path() / "ses-01";

  // Check if the calculated path matches the expected path
  EXPECT_EQ(session.path(), expectedPath);
}

// Test the prefix() method of Session
TEST_F(SessionTest, PrefixTest) {
  // Create a Session object with index 2
  Session session(*subject, 2);

  // Expected prefix: subject id + "_" + session id
  std::string expectedPrefix = subject->id() + "_" + session.id();

  // Check if the calculated prefix matches the expected prefix
  EXPECT_EQ(session.prefix(), expectedPrefix);
}
