#ifndef  READENV_HPP
#define READENV_HPP
#include "IO.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class ReadEnv {
public:
  std::string getenv();
};

#endif
