#ifndef INCLUDE_SUBJECT_HPP_
#define INCLUDE_SUBJECT_HPP_
#include <map>
#include <string>

#include "dataset.hpp"
class Subject {
 public:
  Subject(Dataset const& dataset,
          std::map<std::string, std::string> const& args);

 private:
  Dataset const& dataset_;
};
#endif /* INCLUDE_SUBJECT_HPP_ */
