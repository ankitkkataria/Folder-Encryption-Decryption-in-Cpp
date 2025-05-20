#ifndef PROCESS_MANGAEMENT_HPP
#define PROCESS_MANGAEMENT_HPP

#include "Task.hpp"
#include <memory>
#include <queue>

class ProcessManagement {

private:
  std::queue<std::unique_ptr<Task>> taskQueue;

public:
  ProcessManagement();
  bool submitToQueue(std::unique_ptr<Task> task);
  void executeTasks();
};

#endif 
