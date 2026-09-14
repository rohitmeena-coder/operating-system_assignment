#include "types.h"
#include "stat.h"
#include "user.h"

// Q3 (fix demo): both processes now acquire locks in the same global
// order (Lock1 before Lock2), eliminating circular wait. Same sleep-based
// delay and same workload as the buggy version, but this time it
// completes because circular wait can no longer form.
#define LOCK1 20
#define LOCK2 21
#define PRINTLOCK 22

static void
plog(char *msg)
{
  sem_wait(PRINTLOCK);
  printf(1, "[t=%d] %s\n", uptime(), msg);
  sem_post(PRINTLOCK);
}

int
main(void)
{
  int pid;

  sem_init(LOCK1, 1);
  sem_init(LOCK2, 1);
  sem_init(PRINTLOCK, 1);

  pid = fork();
  if(pid == 0){
    // Process A: Lock1 -> Lock2
    plog("Process A: waiting for Lock1");
    sem_wait(LOCK1);
    plog("Process A: acquired Lock1");
    sleep(5);
    plog("Process A: waiting for Lock2");
    sem_wait(LOCK2);
    plog("Process A: acquired Lock2 -- critical section");
    sem_post(LOCK2);
    sem_post(LOCK1);
    plog("Process A: released both locks, done");
    exit();
  }

  pid = fork();
  if(pid == 0){
    // Process B: Lock1 -> Lock2 (same global order as A -- the fix)
    plog("Process B: waiting for Lock1");
    sem_wait(LOCK1);
    plog("Process B: acquired Lock1");
    sleep(5);
    plog("Process B: waiting for Lock2");
    sem_wait(LOCK2);
    plog("Process B: acquired Lock2 -- critical section");
    sem_post(LOCK2);
    sem_post(LOCK1);
    plog("Process B: released both locks, done");
    exit();
  }

  wait();
  wait();
  printf(1, "RO_FIXED DONE -- both processes completed\n");
  exit();
}
