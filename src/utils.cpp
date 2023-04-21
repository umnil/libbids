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
