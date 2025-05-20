#ifndef TASK_HPP
#define TASK_HPP

#include "../fileHandling/IO.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

enum class Action { ENCRYPT, DECRYPT };

struct Task {
  std::string filepath;
  std::fstream fileStream;
  Action action;
  Task(std::fstream &&stream, Action act, std::string filepath);
  std::string toString();
  static Task fromString(const std::string &taskData);
};

#endif
