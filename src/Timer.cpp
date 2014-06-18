#include "Timer.h"
#include "Util.h"

Timer::Timer() : unit(UnitMs), target(0) {
  zero();
}

Timer::Timer(Unit unit) : unit(unit), target(0) {
  zero();
}

Timer::~Timer() {
}

void Timer::setTarget(uint64_t target) {
  this->target = target;
}

uint64_t Timer::elapsed() {
  uint64_t currentUs = Util::microtime();
  uint64_t elapsed = currentUs - epochUs;
  if (unit == UnitMs)
    elapsed /= 1000;
  return elapsed;
}

uint64_t Timer::remaining() {
  uint64_t elapsedTime = elapsed();
  if (elapsedTime > target)
    return 0;
  return target - elapsedTime;
}

bool Timer::isExpired() {
  return remaining() == 0;
}

void Timer::zero() {
  epochUs = Util::microtime();
}

