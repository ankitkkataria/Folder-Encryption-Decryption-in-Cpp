#include "./src/app/processes/ProcessManagement.hpp"
#include "./src/app/processes/Task.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
  std::string directory;
  std::string actionStr;

  std::cout << "Enter the directory path you want you encrypt/decrypt: ";
  std::getline(std::cin, directory);

  std::cout << "Enter the action you want to perform ENCRYPT/DECRYPT: ";
  std::getline(std::cin, actionStr);

  try {
    if (fs::exists(directory) && fs::is_directory(directory)) {
      ProcessManagement processManagement;
      Action action =
          ((actionStr == "ENCRYPT") ? Action::ENCRYPT : Action::DECRYPT);
      // Iterate over all the files in the directory recursively and make a task
      // for each of them
      for (const auto &entry : fs::recursive_directory_iterator(directory)) {
        std::string filePath = entry.path().string();
        if (entry.is_regular_file()) {
          IO io(filePath);
          std::fstream fileStream = std::move(io.getFileStream());
          if (fileStream.is_open()) {
            std::unique_ptr<Task> task =
                std::make_unique<Task>(std::move(fileStream), action, filePath);
            processManagement.submitToQueue(std::move(task));
          }
        } else {
          std::cout << "Coundn't open the file at location: " << filePath
                    << std::endl;
        }
      }
      processManagement.executeTasks();
    } else {
      std::cout << "Invalid directory path: " << directory << std::endl;
    }
  } catch (const fs::filesystem_error &ex) {
    std::cout << "Filesystem error: " << ex.what() << std::endl;
  }
  return 0;
}
