#include "utils.hpp"

std::string ensure_participant_id(std::string const& id) {
  std::string retval = id;
  if (retval.starts_with("sub-"))
    return retval;
  else
    return std::string("sub-") + retval;
}

std::string ensure_participant_id(int const& id) {
  std::string retval = std::to_string(id);
  return std::string("sub-") +
         std::string(2 - (retval.size() > 2 ? 2 : retval.size()), '0') + retval;
}

std::string trim(std::string const& str) {
  std::string trimmed(str.data());

  // Remove whitespace from the beginning
  size_t start = trimmed.find_first_not_of(" \t\n\r\f\v");
  if (start != std::string::npos) {
    trimmed = trimmed.substr(start);
  } else {
    trimmed = "";
  }

  // Remove whitespace from the end
  size_t end = trimmed.find_last_not_of(" \t\n\r\f\v");
  if (end != std::string::npos) {
    trimmed = trimmed.substr(0, end + 1);
  }

  return trimmed;
}
