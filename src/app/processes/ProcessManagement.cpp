#include "ProcessManagement.hpp"
#include "Task.hpp"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <sys/wait.h>
#include "../encryptDecrypt/Cryption.hpp"

ProcessManagement::ProcessManagement() {}

bool ProcessManagement::submitToQueue(std::unique_ptr<Task> task) {
  taskQueue.push(std::move(task));
  return true;
}

void ProcessManagement::executeTasks() {
  std::unique_ptr<Task> taskToExecute = std::move(taskQueue.front());
  taskQueue.pop();
  executeCryption(taskToExecute->toString());
  std::cout << "Executing the task "  << taskToExecute->toString() << std::endl;
}
