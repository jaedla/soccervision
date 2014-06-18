#ifndef WORKPRODUCER_H
#define WORKPRODUCER_H

#include "Set.h"
#include "Worker.h"

template<typename T>
class WorkProducer {
private:
  typedef Worker<T> Worker;
public:
  void addWorker(sp<Worker> worker) {
    workers.insert(worker);
  }
protected:
  void newWorkProduced(sp<T> work) {
    for (auto worker : workers)
      worker->addWork(work);
  }
private:
  Set<sp<Worker> > workers;
};

#endif
