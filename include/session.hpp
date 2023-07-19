#ifndef INCLUDE_SESSION_HPP_
#define INCLUDE_SESSION_HPP_

#include <filesystem>
#include <string>

#include "entity.hpp"
#include "subject.hpp"

// class Subject;

class Session : public Entity {
 public:
  /**
   * Initialize a new session for a giben subject
   * @brief Default Constructor
   * @param subject The subject object that will own this session
   * @param idx The session index number or identifier
   */
  // Session(Subject const& subject, int idx);
  Session(Subject subject, int idx);

  std::filesystem::path path(void) const;
  std::string prefix(void) const;

  Subject subject(void) const;

 private:
  // Subject const& subject_;
  Subject subject_;
};

#endif /* INCLUDE_SESSION_HPP_ */
