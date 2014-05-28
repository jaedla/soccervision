#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <string>

class Thread {

public:
  Thread(std::string name);
  virtual ~Thread();

  int start();
  int join();
  int detach();
  pthread_t self();

  virtual void *run() = 0;

private:
  pthread_t handle;
  bool running;
  bool detached;
  std::string name;
};

#endif // THREAD_H
