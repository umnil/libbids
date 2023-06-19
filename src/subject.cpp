#include "subject.hpp"

Subject::Subject(Dataset const& dataset,
                 std::map<std::string, std::any> const& args)
    : dataset_(dataset), properties_(args) {}
