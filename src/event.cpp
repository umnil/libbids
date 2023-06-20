#include <algorithm>
#include <random>

#include "event.hpp"

Event::Event(std::chrono::milliseconds onset,
             std::chrono::milliseconds duration, std::string trial_type)
    : onset_(onset), duration_(duration), trial_type_(trial_type) {}

std::chrono::milliseconds Event::onset() const { return onset_; }

std::chrono::milliseconds Event::duration() const { return duration_; }

std::string Event::trial_type() const { return trial_type_; }

std::vector<int> Event::sample_onsets(const std::vector<Event>& events,
                                      int sfreq) {
  std::vector<int> sample_onsets;
  sample_onsets.reserve(events.size());

  for (const auto& event : events) {
    int time_onset = static_cast<int>(event.onset().count());
    sample_onsets.push_back(time_onset * sfreq);
  }

  return sample_onsets;
}

std::vector<Event> Event::generate_fixed_duration_events(
    std::chrono::milliseconds duration,
    const std::vector<std::string>& trial_types,
    std::chrono::milliseconds task_duration) {
  int n_events = static_cast<int>(task_duration.count() / duration.count());
  std::vector<Event> events;
  events.reserve(n_events);

  for (int i = 0; i < n_events; ++i) {
    std::chrono::milliseconds onset(i * duration.count());
    std::string trial_type = trial_types[i % trial_types.size()];
    events.emplace_back(onset, duration, trial_type);
  }

  return events;
}

std::vector<Event> Event::generate_variable_duration_events(
    std::chrono::milliseconds min_duration,
    std::chrono::milliseconds max_duration,
    const std::vector<std::string>& trial_types,
    std::chrono::milliseconds task_duration) {
  int max_n_events =
      static_cast<int>(task_duration.count() / min_duration.count());
  std::vector<double> event_durations_array(max_n_events);
  std::vector<std::chrono::milliseconds> event_durations;
  event_durations.reserve(max_n_events);

  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_real_distribution<double> distribution(0.0, 1.0);
  double shift = static_cast<double>(min_duration.count());
  double scale =
      static_cast<double>(max_duration.count() - min_duration.count());

  for (auto& duration : event_durations_array) {
    duration = distribution(generator) * scale + shift;
    event_durations.push_back(
        std::chrono::milliseconds(static_cast<long long>(duration)));
  }

  std::vector<double> event_onsets_array = {0.0};
  std::partial_sum(event_durations.begin(), event_durations.end() - 1,
                   std::back_inserter(event_onsets_array));
  std::vector<std::chrono::milliseconds> event_onsets;
  event_onsets.reserve(max_n_events);

  for (const auto& onset : event_onsets_array) {
    event_onsets.push_back(
        std::chrono::milliseconds(static_cast<long long>(onset)));
  }

  int n_types = static_cast<int>(trial_types.size());
  int n_repeats =
      static_cast<int>(std::ceil(static_cast<double>(max_n_events) / n_types));
  std::vector<std::string> event_types;
  event_types.reserve(max_n_events);

  if (n_types > 2) {
    std::vector<std::vector<std::string>> list_events;
    list_events.reserve(n_repeats);

    std::vector<std::string> type_set(trial_types.begin(), trial_types.end());
    std::random_shuffle(type_set.begin(), type_set.end());
    list_events.push_back(type_set);

    for (int i = 0; i < n_repeats - 1; ++i) {
      type_set.clear();
      type_set.assign(trial_types.begin(), trial_types.end());
      std::random_shuffle(type_set.begin(), type_set.end());

      while (type_set[0] == list_events.back().back()) {
        type_set.clear();
        type_set.assign(trial_types.begin(), trial_types.end());
        std::random_shuffle(type_set.begin(), type_set.end());
      }

      list_events.push_back(type_set);
    }

    for (const auto& event_set : list_events) {
      event_types.insert(event_types.end(), event_set.begin(), event_set.end());
    }
  } else {
    event_types.reserve(n_repeats * n_types);

    for (int i = 0; i < n_repeats; ++i) {
      for (const auto& trial_type : trial_types) {
        event_types.push_back(trial_type);
      }
    }
  }

  std::vector<Event> events;
  events.reserve(max_n_events);

  for (int i = 0; i < max_n_events; ++i) {
    std::chrono::milliseconds onset = event_onsets[i];
    std::chrono::milliseconds duration = event_durations[i];
    std::string trial_type = event_types[i];
    events.emplace_back(onset, duration, trial_type);
  }

  return events;
}
