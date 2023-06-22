#ifndef INCLUDE_SESSION_HPP_
#define INCLUDE_SESSION_HPP_

#include <filesystem>
#include <string>

#include "entity.hpp"

class Subject;

class Session : public Entity {
 public:
  Session(Subject const& subject, int idx);

  std::filesystem::path path() const;
  std::string get_prefix() const;

 private:
  Subject const& subject_;
};

#endif /* INCLUDE_SESSION_HPP_ */
