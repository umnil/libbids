#include <gtest/gtest.h>

#include "event.hpp"

// Test fixture for Event class
class EventTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Set up common variables or objects for the tests
  }

  void TearDown() override {
    // Clean up any temporary resources after the tests
  }
};

TEST_F(EventTest, DefaultConstructor) {
  Event event;
  EXPECT_EQ(event.onset().count(), 0);
  EXPECT_EQ(event.duration().count(), 0);
  EXPECT_EQ(event.trial_type(), "");
}

TEST_F(EventTest, ConstructorWithParameters) {
  std::chrono::milliseconds onset(100);
  std::chrono::milliseconds duration(200);
  std::string trial_type = "test";
  Event event(onset, duration, trial_type);
  EXPECT_EQ(event.onset(), onset);
  EXPECT_EQ(event.duration(), duration);
  EXPECT_EQ(event.trial_type(), trial_type);
}

TEST_F(EventTest, SampleOnsets) {
  std::vector<Event> events;
  events.emplace_back(std::chrono::milliseconds(100),
                      std::chrono::milliseconds(200), "A");
  events.emplace_back(std::chrono::milliseconds(300),
                      std::chrono::milliseconds(400), "B");
  events.emplace_back(std::chrono::milliseconds(500),
                      std::chrono::milliseconds(600), "C");

  int sfreq = 100;
  std::vector<int> sample_onsets = Event::sample_onsets(events, sfreq);

  EXPECT_EQ(sample_onsets.size(), events.size());
  EXPECT_EQ(sample_onsets[0], 10000);  // 100 * 100
  EXPECT_EQ(sample_onsets[1], 30000);  // 300 * 100
  EXPECT_EQ(sample_onsets[2], 50000);  // 500 * 100
}

TEST_F(EventTest, GenerateFixedDurationEvents) {
  std::chrono::milliseconds duration(100);
  std::vector<std::string> trial_types = {"A", "B", "C"};
  std::chrono::milliseconds task_duration(500);

  std::vector<Event> events = Event::generate_fixed_duration_events(
      duration, trial_types, task_duration);

  EXPECT_EQ(events.size(), 5);  // task_duration / duration

  // Check the generated events
  EXPECT_EQ(events[0].onset().count(), 0);
  EXPECT_EQ(events[0].duration().count(), 100);
  EXPECT_EQ(events[0].trial_type(), "A");

  EXPECT_EQ(events[1].onset().count(), 100);
  EXPECT_EQ(events[1].duration().count(), 100);
  EXPECT_EQ(events[1].trial_type(), "B");

  EXPECT_EQ(events[2].onset().count(), 200);
  EXPECT_EQ(events[2].duration().count(), 100);
  EXPECT_EQ(events[2].trial_type(), "C");

  EXPECT_EQ(events[3].onset().count(), 300);
  EXPECT_EQ(events[3].duration().count(), 100);
  EXPECT_EQ(events[3].trial_type(), "A");

  EXPECT_EQ(events[4].onset().count(), 400);
  EXPECT_EQ(events[4].duration().count(), 100);
  EXPECT_EQ(events[4].trial_type(), "B");
}

TEST_F(EventTest, GenerateVariableDurationEvents) {
  std::chrono::milliseconds min_duration(100);
  std::chrono::milliseconds max_duration(300);
  std::vector<std::string> trial_types = {"A", "B", "C"};
  std::chrono::milliseconds task_duration(1000);

  std::vector<Event> events = Event::generate_variable_duration_events(
      min_duration, max_duration, trial_types, task_duration);

  // Check the number of generated events
  int expected_num_events =
      static_cast<int>(task_duration.count() / min_duration.count());
  EXPECT_EQ(events.size(), expected_num_events);

  // Check the trial types of generated events
  std::vector<std::string> generated_types;
  for (auto const& event : events) {
    generated_types.push_back(event.trial_type());
  }

  for (auto const& trial_type : trial_types) {
    EXPECT_TRUE(std::find(generated_types.begin(), generated_types.end(),
                          trial_type) != generated_types.end());
  }
}
