/// Equivalent to `trigger.sh`.
/// For the ease of implementation, we compile the test case into a single
/// executable, instead of using a shellscript.

#include "src/main.h"
#include "main.h"
#include "pthread.h"
#include "signal.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

int aget_exited = 0;

void *sr_deadlock_detector(void *unused) {
  struct timespec until, cur;
  clock_gettime(CLOCK_REALTIME, &until);
  until.tv_sec += 5; // If the program does not finish in 5 seconds, it is
                     // considered a deadlock. I was serious when I said it was
                     // a basic deadlock detector.

  while (1) { // Using busy-wait because it iterferes with the usage of SIGALRM
              // in aget.
    clock_gettime(CLOCK_REALTIME, &cur);
    if (cur.tv_sec == until.tv_sec) {
      break;
    }
    if (aget_exited == 1) {
      return NULL;
    }
  }

  // This can actually cause undefined behavior but we don't care at this time.
  if (aget_exited == 0) {
    abort();
    return NULL;
  }
  return NULL;
}

int main() {
  system("rm test100k.db"); // Cleanup.
  int argc = 3;
  char *argv[4] = {"aget", "http://speedtest.ftp.otenet.gr/files/test100k.db",
                   "-n2", NULL};
  real_main(argc, argv); // Run the original program.
  aget_exited = 1;
  system("rm test100k.db"); // Cleanup.

  return 0;
}