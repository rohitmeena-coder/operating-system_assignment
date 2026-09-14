#include "types.h"
#include "stat.h"
#include "user.h"

// Q3 (problem demo): Process A acquires Lock1 then Lock2.
// Process B acquires Lock2 then Lock1. Each process sleeps (yields the
// CPU for several timer ticks) between its two acquisitions -- this
// guarantees a real context switch happens in the window, so the other
// process reliably grabs the opposite lock first and circular wait forms.
// (A pure busy-wait loop finished inside a single scheduler quantum and
// did not reproduce the race, so sleep() is used instead of delay-looping.)
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
    // Process B: Lock2 -> Lock1  (reverse order -- this is the bug)
    plog("Process B: waiting for Lock2");
    sem_wait(LOCK2);
    plog("Process B: acquired Lock2");
    sleep(5);
    plog("Process B: waiting for Lock1");
    sem_wait(LOCK1);
    plog("Process B: acquired Lock1 -- critical section");
    sem_post(LOCK1);
    sem_post(LOCK2);
    plog("Process B: released both locks, done");
    exit();
  }

  wait();
  wait();
  printf(1, "RO_BAD DONE -- both processes completed\n");
  exit();
}
