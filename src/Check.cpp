#include <Check.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void checkInternal(bool condition, const char *msg, const char *file, int line, ...) {
  va_list va;
  if (!condition) {
    char wholeMsg[4096];
    va_start(va, line);
    vsnprintf(wholeMsg, sizeof(wholeMsg), msg, va);
    printf("%s:%d: Check failed: %s\n", file, line, wholeMsg);
    va_end(va);
    exit(1);
  }
}

