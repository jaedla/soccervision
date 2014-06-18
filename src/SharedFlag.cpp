#include "SharedFlag.h"

#include "ScopedMutex.h"

SharedFlag::SharedFlag() : flag(false) {
}

SharedFlag::SharedFlag(bool value) : flag(value) {
}

SharedFlag::~SharedFlag() {
}

bool SharedFlag::get() {
  ScopedMutex lock(&mutex);
  return flag;
}

void SharedFlag::set(bool newValue) {
  ScopedMutex lock(&mutex);
  flag = newValue;
}

bool SharedFlag::is(bool value) {
  ScopedMutex lock(&mutex);
  return flag == value;
}

