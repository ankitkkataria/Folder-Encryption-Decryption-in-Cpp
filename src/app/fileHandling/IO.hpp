#ifndef IO_HPP
#define IO_HPP

#include <fstream>
#include <iostream>
#include <string>

class IO {
private:
  std::fstream fileStream;

public:
  IO(const std::string &filepath);
  ~IO();
  std::fstream getFileStream();
};

#endif // !IO_HPP
