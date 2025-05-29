#ifndef PROCESS_MANGAEMENT_HPP
#define PROCESS_MANGAEMENT_HPP

#include "Task.hpp"
#include <atomic>
#include <memory>
#include <mutex>
#include <queue>
#include <semaphore.h>

class ProcessManagement {
private:
  // Shared memory structure for inter-process communication
  struct SharedMemory {
    std::atomic<int> size;
    char tasks[1000][256]; // Store 1000 tasks in string form
    int front;
    int rear;
  };

  SharedMemory *sharedMem;
  int shmFd;
  const char* SHM_NAME = "/my_queue";

  // Semaphores & Mutex for synchronization
  sem_t* itemsSemaphore;
  sem_t* emptySlotsSemaphore;
  std::mutex queueLock;

public:
  ProcessManagement();
  ~ProcessManagement();
  bool submitToQueue(std::unique_ptr<Task> task);
  void executeTasks();
};

#endif
