#include "IO.hpp"
#include <fstream>
#include <iostream>

IO::IO(const std::string &filepath) {
  fileStream.open(filepath, std::ios::in | std::ios::out | std::ios::binary);
  if (!fileStream.is_open()) {
    std::cout << "Couldn't open the file at the path " << filepath << std::endl;
  }
}

std::fstream IO::getFileStream() { return std::move (fileStream); }

IO::~IO() {
  if (fileStream.is_open()) {
    fileStream.close();
  }
}
