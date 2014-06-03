#ifndef CHECK_H
#define CHECK_H

void checkInternal(bool condition, const char *msg, const char *file, int line, ...);
#define Check(condition, msg, ...) checkInternal((condition), (msg), __FILE__, __LINE__, ##__VA_ARGS__);

#endif
