#include "Task.hpp"
#include "../fileHandling/IO.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

Task::Task(std::fstream &&stream, Action act, std::string filepath) {
  this->fileStream = std::move(stream);
  this->action = act;
  this->filepath = filepath;
}

// Serialization function
std::string Task ::toString() {
  std::ostringstream oss;
  oss << filepath << "," << ((action == Action::ENCRYPT) ? "ENCRYPT" : "DECRYPT");
  return oss.str();
}

Task Task ::fromString(const std::string &taskData) {
  std::istringstream iss(taskData);
  std::string actionStr;
  std::string filePath;
  if (std::getline(iss, filePath, ',') && std::getline(iss, actionStr)) {
    Action action =
        (actionStr == "ENCRYPT") ? Action::ENCRYPT : Action::DECRYPT;
    IO io(filePath);
    std::fstream fileStream = std::move(io.getFileStream());
    if (fileStream.is_open()) {
      return Task(std::move(fileStream), action, filePath);
    } else {
      throw std::runtime_error("Failed to open the file at path: " + filePath);
    }
  } else {
    throw std::runtime_error("Invalid Taskdata format");
  }
}
