#include "session.hpp"
#include "subject.hpp"

// Session::Session(Subject const& subject, int idx)
Session::Session(Subject subject, int idx)
    : Entity("Session", std::nullopt, idx), subject_(subject) {
  std::filesystem::create_directory(this->path());
}

std::filesystem::path Session::path() const {
  std::filesystem::path subject_path = this->subject_.path();
  return subject_path / this->id();
}

std::string Session::prefix() const {
  return this->subject_.id() + "_" + this->id();
}

Subject Session::subject(void) const { return this->subject_; }
