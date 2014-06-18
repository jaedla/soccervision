#ifndef TIMER_H
#define TIMER_H

#include <cstdint>

class Timer {
public:
  enum Unit {
    UnitMs,
    UnitUs,
  };
  Timer();
  Timer(Unit units);
  ~Timer();
  void setTarget(uint64_t target);
  uint64_t elapsed();
  uint64_t remaining();
  bool isExpired();
  void zero();
private:
  Unit unit;
  uint64_t epochUs;
  uint64_t target;
};

#endif
