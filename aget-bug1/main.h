#include "pthread.h"
#include "signal.h"
#include "stdio.h"

extern pthread_mutex_t mtx;
extern int aget_exited;

void *sr_deadlock_detector(void *unused);
