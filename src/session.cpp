#include "session.hpp"
#include "subject.hpp"

Session::Session(Subject const& subject, int idx)
    : Entity("Session", std::nullopt, idx), subject_(subject) {
  std::filesystem::create_directory(this->path());
}

std::filesystem::path Session::path() const {
  return this->subject_.path() / this->id();
}

std::string Session::get_prefix() const {
  return this->subject_.id() + "_" + this->id();
}
