#ifndef INCLUDE_SESSION_HPP_
#define INCLUDE_SESSION_HPP_

#include <filesystem>
#include <string>

#include "entity.hpp"

class Subject;

class Session : public Entity {
 public:
  /**
   * Initialize a new session for a giben subject
   * @brief Default Constructor
   * @param subject The subject object that will own this session
   * @param idx The session index number or identifier
   */
  Session(Subject const& subject, int idx);

  std::filesystem::path path() const;
  std::string prefix() const;

 private:
  Subject const& subject_;
};

#endif /* INCLUDE_SESSION_HPP_ */
