#include "IO.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "ReadEnv.hpp"

std::string ReadEnv::getenv() {
  std::string filePath = ".env";
  IO io(filePath);
  std::fstream fileStream = io.getFileStream();
  std::stringstream buffer;
  buffer << fileStream.rdbuf();
  std::string content = buffer.str();
  return content;
}
