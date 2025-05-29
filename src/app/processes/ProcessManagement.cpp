#include "ProcessManagement.hpp"
#include "../encryptDecrypt/Cryption.hpp"
#include "Task.hpp"
#include <algorithm>
#include <atomic>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <mutex>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

ProcessManagement::ProcessManagement() {
  // Initialize semaphores
  itemsSemaphore = sem_open("/items_semaphore", O_CREAT, 0666, 0);
  emptySlotsSemaphore = sem_open("/empty_slots_semaphore", O_CREAT, 0666, 1000);

  // Initialize shared memory
  shmFd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
  ftruncate(shmFd, sizeof(SharedMemory));
  sharedMem = static_cast<SharedMemory *>(
      mmap(nullptr, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0));

  // Initialize shared memory structure
  sharedMem->size.store(0);
  sharedMem->front = 0;
  sharedMem->rear = 0;
}

ProcessManagement::~ProcessManagement() {
  munmap(sharedMem, sizeof(SharedMemory));
  shm_unlink(SHM_NAME);
  sem_close(itemsSemaphore);
  sem_close(emptySlotsSemaphore);
  sem_unlink("/items_semaphore");
  sem_unlink("/empty_slots_semaphore");
}

bool ProcessManagement::submitToQueue(std::unique_ptr<Task> task) {
  sem_wait(emptySlotsSemaphore);
  std::unique_lock<std::mutex> lock(queueLock);
  
  if (sharedMem->size.load() >= 1000) {
    std::cout << "Circular queue is full!" << std::endl;
    return false;
  }
  
  strcpy(sharedMem->tasks[sharedMem->rear], task->toString().c_str());
  sharedMem->rear = (sharedMem->rear + 1) % 1000;
  sharedMem->size.fetch_add(1);
  
  lock.unlock();
  sem_post(itemsSemaphore);

  int pid = fork();
  if (pid < 0) {
    return false;
  } else if (pid == 0) {
    // Child process
    executeTasks();
    exit(0);
  }
  
  return true;
}

void ProcessManagement::executeTasks() {
  sem_wait(itemsSemaphore);
  std::unique_lock<std::mutex> lock(queueLock);
  
  char taskStr[256];
  strcpy(taskStr, sharedMem->tasks[sharedMem->front]);
  sharedMem->front = (sharedMem->front + 1) % 1000;
  sharedMem->size.fetch_sub(1);
  
  lock.unlock();
  sem_post(emptySlotsSemaphore);

  executeCryption(taskStr);
}
