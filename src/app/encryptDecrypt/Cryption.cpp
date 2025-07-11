#include "Cryption.hpp"
#include "../fileHandling/ReadEnv.hpp"
#include "../processes/Task.hpp"
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

int executeCryption(const std::string &taskData) {
  Task task = Task::fromString(taskData);
  ReadEnv env;
                  
  std::string envKey = env.getenv();
  int key = std::stoi(envKey);

  if (task.action == Action::ENCRYPT) {
    char ch;
    while (task.fileStream.get(ch)) {
      ch = (ch + key) % 256;
      task.fileStream.seekp(-1, std::ios ::cur);
      task.fileStream.put(ch);
    }
  } else {
    char ch;
    while (task.fileStream.get(ch)) {
      ch = (ch - key + 256) % 256;
      task.fileStream.seekp(-1, std::ios ::cur);
      task.fileStream.put(ch);
    }
  }
  task.fileStream.close();
  
  /*std::time_t t = std::time(nullptr);*/
  /*std::tm* now = std::localtime(&t);*/
  /*std::cout << "Completed encryption/decryption at: " << std::put_time(now, "%Y-%m-%d %H:%M:%S") << std::endl;*/
  
  return 0;
}
