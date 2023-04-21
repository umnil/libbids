#include "subject.hpp"

Subject::Subject(Dataset const& dataset,
                 std::map<std::string, std::string> const& args)
    : dataset_(dataset) {}
