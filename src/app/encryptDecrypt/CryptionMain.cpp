#include "Cryption.hpp"
#include <iostream>
#include <system_error>
int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage ./cryption <task_data_string e.g. text_file_path,ENCRYPT or text_file_path,DECRYPT>" << std::endl;
  }
  executeCryption(argv[1]);
  return 0;
}
