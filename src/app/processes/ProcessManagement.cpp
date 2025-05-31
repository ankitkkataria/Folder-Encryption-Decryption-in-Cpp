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
#include <shared_mutex>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

ProcessManagement::ProcessManagement() {
  // Initialize semaphores - These semaphores use mmap files to store their data that are shared among processes
  itemsSemaphore = sem_open("/items_semaphore", O_CREAT, 0666, 0);
  emptySlotsSemaphore = sem_open("/empty_slots_semaphore", O_CREAT, 0666, 1000);
  queueAccessSemaphore = sem_open("/queue_access_semaphore", O_CREAT, 0666, 1); // Creating a binary semaphore for process synchronization cause mutexes only handle thread synchornization and each mutex will just get a new copy for each process.

  // Initialize shared memory
  shmFd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
  ftruncate(shmFd, sizeof(SharedMemory));
  sharedMem = static_cast<SharedMemory *>(
      mmap(nullptr, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0));

  // Initialize shared memory structure - size is atomic
  sharedMem->size.store(0);
  sharedMem->front = 0;
  sharedMem->rear = 0;
}

ProcessManagement::~ProcessManagement() {
  munmap(sharedMem, sizeof(SharedMemory));
  shm_unlink(SHM_NAME);
  sem_close(queueAccessSemaphore);
  sem_close(itemsSemaphore);
  sem_close(emptySlotsSemaphore);
  sem_unlink("/items_semaphore");
  sem_unlink("/empty_slots_semaphore");
  sem_unlink("/queue_access_semaphore");
}

bool ProcessManagement::submitToQueue(std::unique_ptr<Task> task) {
  // Not using taskQueue anymore as we have shared memory queue
  sem_wait(emptySlotsSemaphore);
  sem_wait(queueAccessSemaphore);
  /*std::unique_lock<std::mutex> lock(queueLock);*/
  
  if (sharedMem->size.load() >= 1000) {
    std::cout << "Circular queue is full!" << std::endl;
    return false;
  }
  
  strcpy(sharedMem->tasks[sharedMem->rear], task->toString().c_str());
  sharedMem->rear = (sharedMem->rear + 1) % 1000;
  sharedMem->size.fetch_add(1);
  /*lock.unlock();*/
  sem_post(queueAccessSemaphore);
  sem_post(itemsSemaphore);
  int pid = fork();
  if (pid < 0) {
    return false;
  } else if (pid == 0) {
    // Child process
    /*std::cout << "Entering child process" << std::endl;*/
    executeTasks();
    /*std::cout << "Exiting child process" << std::endl;*/
    exit(0);
  }
  return true;
}

void ProcessManagement::executeTasks() {
  // Producer-consumer synchronization - wait for an item to be available
  sem_wait(itemsSemaphore);
  sem_wait(queueAccessSemaphore);
  /*std::unique_lock<std::mutex> lock(queueLock);*/
  
  char taskStr[256];
  strcpy(taskStr, sharedMem->tasks[sharedMem->front]);
  sharedMem->front = (sharedMem->front + 1) % 1000;
  sharedMem->size.fetch_sub(1);
  
  /*lock.unlock();*/
  sem_post(queueAccessSemaphore);  // Signal that a slot is now available
  sem_post(emptySlotsSemaphore);  // Signal that a slot is now available

  executeCryption(taskStr);
  /*std::cout << "Completed task: " << taskStr << std::endl;*/
}

// Just something you should know 
// std::mutex can't be used for process synchronization
// but posix threads can be

// In your SharedMemory structure
/*struct SharedMemory {*/
/*    pthread_mutex_t queueMutex;*/
/*    // ... other members*/
/*};*/

// Initialize for process sharing
/*pthread_mutexattr_t attr;*/
/*pthread_mutexattr_init(&attr);*/
/*pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);  // Key line!*/
/*pthread_mutex_init(&sharedMem->queueMutex, &attr);*/
/**/
/*// Now all processes can use it*/
/*pthread_mutex_lock(&sharedMem->queueMutex);*/
/*// Critical section*/
/*pthread_mutex_unlock(&sharedMem->queueMutex);*/

// But still binary semaphores are preferred.
