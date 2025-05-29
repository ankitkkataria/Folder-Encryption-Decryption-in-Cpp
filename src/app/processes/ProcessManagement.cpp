#include "ProcessManagement.hpp"
#include "../encryptDecrypt/Cryption.hpp"
#include "Task.hpp"
#include <algorithm>
#include <atomic>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <iterator>
#include <mutex>
#include <ostream>
#include <semaphore.h>
#include <shared_mutex>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

ProcessManagement::ProcessManagement() {
  itemsSemaphore =
      sem_open("/items_semaphore", O_CREAT, 0666,
               0); // These semaphores also use these mmap files to store their
                   // data that are shared among processes
  emptySlotsSemaphore =
      sem_open("/empty_slots_semaphore", O_CREAT, 0666,
               1000); // These semaphores also use these mmap files to store
                      // their data that are shared among processes

  shmFd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
  ftruncate(shmFd, sizeof(SharedMemory));
  sharedMem = static_cast<SharedMemory *>(mmap(nullptr, sizeof(SharedMemory),
                                               PROT_READ | PROT_WRITE,
                                               MAP_SHARED, shmFd, 0));

  // We're treating size different cause it is a atomic variable
  sharedMem->size.store(0);
  sharedMem->front = 0;
  sharedMem->rear = 0;
}

bool ProcessManagement::submitToQueue(std::unique_ptr<Task> task) {
  /*taskQueue.push(std::move(task));*/ // Not neccessary now cause we have a new
                                       // queue that uses the sharedMem struct
  sem_wait(emptySlotsSemaphore);
  std::unique_lock<std::mutex> lock(queueLock);
  if (sharedMem->size.load() >= 1000) {
    std::cout << "Circular queue is full!" << std::endl;
    return false;
  } else {
    strcpy(sharedMem->tasks[sharedMem->rear], task->toString().c_str());
    sharedMem->rear = (sharedMem->rear + 1) % 1000;
    sharedMem->size.fetch_add(1);
  }
  lock.unlock();
  sem_post(itemsSemaphore);

  int pid = fork();

  if (pid < 0) {
    return false;
  } else if (pid > 0) {
    /*std::cout << "Entering the parent process" << std::endl;*/
  } else {
    /*std::cout << "Entering the child process" << std::endl;*/
    executeTasks();
    /*std::cout << "Exiting the child process" << std::endl;*/
    exit(0);
  }
  return true;
}

void ProcessManagement::executeTasks() {
  sem_wait(itemsSemaphore); // Producer consumer problem basic like we need some
                            // element in the queue to be able to access it in
                            // the first place.
  std::unique_lock<std::mutex> lock(queueLock);
  char taskStr[256];
  strcpy(taskStr, sharedMem->tasks[sharedMem->front]);
  sharedMem->front = (sharedMem->front + 1) % 1000;
  sharedMem->size.fetch_sub(1);
  /*// Original code with normal queue original vode had a while loop around it
   * but we don't need the commented code block below but we don't need it now
   * cause we're spinning up individual child processes for each task*/
  /*std::unique_ptr<Task> taskToExecute = std::move(taskQueue.front());*/
  /*taskQueue.pop();*/
  /*executeCryption(taskToExecute->toString());*/
  /*std::cout << "Executing the task " << taskToExecute->toString() <<
   * std::endl;*/
  lock.unlock();
  sem_post(
      emptySlotsSemaphore); // Delete the element from the queue as it has been
                            // consumed now queue can accept more enteries

  executeCryption(taskStr);
  /*std::cout << "Executing the task " << taskStr << std::endl;*/
}

ProcessManagement ::~ProcessManagement() {
  munmap(sharedMem, sizeof(SharedMemory));
  shm_unlink(SHM_NAME);
}
