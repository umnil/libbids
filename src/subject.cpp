#include "subject.hpp"

Subject::Subject(Dataset const& dataset,
                 std::map<std::string, std::string> const& args)
    : dataset_(dataset), properties_(args) {}

std::map<std::string, std::string> const& Subject::to_dict(void) const {
  return this->properties_;
}
