#include "ProcessManagement.hpp"
#include "../encryptDecrypt/Cryption.hpp"
#include "Task.hpp"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <sys/wait.h>

ProcessManagement::ProcessManagement() {}

bool ProcessManagement::submitToQueue(std::unique_ptr<Task> task) {
  taskQueue.push(std::move(task));
  return true;
}

void ProcessManagement::executeTasks() {
  /*std::cout << "Task queue size: " << taskQueue.size() << std::endl;*/
  while (!taskQueue.empty()) {
    std::unique_ptr<Task> taskToExecute = std::move(taskQueue.front());
    taskQueue.pop();
    executeCryption(taskToExecute->toString());
    /*std::cout << "Executing the task " << taskToExecute->toString()*/
    /*          << std::endl;*/
  }
}
