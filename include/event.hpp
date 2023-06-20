#ifndef EVENT_HPP_
#define EVENT_HPP_

#include <chrono>
#include <string>
#include <vector>

class Event {
 public:
  Event(std::chrono::milliseconds onset = std::chrono::milliseconds(0),
        std::chrono::milliseconds duration = std::chrono::milliseconds(0),
        std::string trial_type = "");

  std::chrono::milliseconds onset() const;
  std::chrono::milliseconds duration() const;
  std::string trial_type() const;

  static std::vector<int> sample_onsets(const std::vector<Event>& events,
                                        int sfreq);

  static std::vector<Event> generate_fixed_duration_events(
      std::chrono::milliseconds duration,
      const std::vector<std::string>& trial_types,
      std::chrono::milliseconds task_duration);

  static std::vector<Event> generate_variable_duration_events(
      std::chrono::milliseconds min_duration,
      std::chrono::milliseconds max_duration,
      const std::vector<std::string>& trial_types,
      std::chrono::milliseconds task_duration);

 private:
  std::chrono::milliseconds onset_;
  std::chrono::milliseconds duration_;
  std::string trial_type_;
};

#endif /* EVENT_HPP_ */
