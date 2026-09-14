#include "types.h"
#include "stat.h"
#include "user.h"

// Q4: Generalized Dining-Philosophers-style problem.
// 5 processes, each needs 2 of 3 resource types to do a task:
//   R0 = printer (2 instances)
//   R1 = scanner (1 instance)
//   R2 = disk    (2 instances)
// Counting semaphores (from the assignment-5 sem_init/sem_wait/sem_post
// syscalls) model each resource pool's instance count directly.
//
// Deadlock-avoidance strategy: RESOURCE ORDERING (reused from Q3).
// Every process always requests its lower-numbered resource type before
// its higher-numbered one. Since all 5 processes follow the same global
// order (R0 < R1 < R2), a circular wait can never form -- the necessary
// condition for deadlock is structurally eliminated, regardless of how
// the scheduler interleaves them.
#define NPROC 5
#define CYCLES 4

#define PRINTER 0
#define SCANNER 1
#define DISK    2

static char *rname[3] = {"printer", "scanner", "disk"};

// Each process is assigned 2 of the 3 resource types. first/second is
// already stored in resource-ordering order (first < second).
static int need_a[NPROC] = {PRINTER, SCANNER, PRINTER, PRINTER, SCANNER};
static int need_b[NPROC] = {SCANNER, DISK,    DISK,    SCANNER, DISK};

struct shared_log {
  int completed[NPROC];
  int active[3]; // instances currently in use, for the overrun check
  int overrun;   // set to 1 if any pool ever exceeds its capacity
};

static int capacity[3] = {2, 1, 2};

// Console writes from concurrent processes can interleave character by
// character; a binary semaphore (PRINTLOCK) held only around each single
// printf call keeps the log readable without affecting the
// resource-ordering logic being demonstrated.
#define PRINTLOCK 19

static void
work(void)
{
  // A real sleep (rather than a busy-wait) forces a genuine context
  // switch, so all 5 processes visibly interleave their requests and
  // releases instead of running one after another to completion.
  sleep(2);
}

static void
worker(struct shared_log *log, int id)
{
  int a = need_a[id];
  int b = need_b[id];
  int c;

  for(c = 0; c < CYCLES; c++){
    sem_wait(PRINTLOCK);
    printf(1, "P%d: requesting %s (cycle %d)\n", id, rname[a], c + 1);
    sem_post(PRINTLOCK);
    sem_wait(10 + a);
    log->active[a]++;
    if(log->active[a] > capacity[a])
      log->overrun = 1;
    sem_wait(PRINTLOCK);
    printf(1, "P%d: granted %s\n", id, rname[a]);
    sem_post(PRINTLOCK);

    sem_wait(PRINTLOCK);
    printf(1, "P%d: requesting %s (cycle %d)\n", id, rname[b], c + 1);
    sem_post(PRINTLOCK);
    sem_wait(10 + b);
    log->active[b]++;
    if(log->active[b] > capacity[b])
      log->overrun = 1;
    sem_wait(PRINTLOCK);
    printf(1, "P%d: granted %s\n", id, rname[b]);
    sem_post(PRINTLOCK);

    sem_wait(PRINTLOCK);
    printf(1, "P%d: working with %s + %s\n", id, rname[a], rname[b]);
    sem_post(PRINTLOCK);
    work();

    log->active[b]--;
    sem_post(10 + b);
    log->active[a]--;
    sem_post(10 + a);
    sem_wait(PRINTLOCK);
    printf(1, "P%d: released %s + %s\n", id, rname[a], rname[b]);
    sem_post(PRINTLOCK);

    log->completed[id]++;
  }
}

int
main(void)
{
  struct shared_log *log = (struct shared_log*)shmget();
  int i, pid;

  if((uint)log == (uint)-1){
    printf(1, "syncdeadlock: shmget failed\n");
    exit();
  }
  for(i = 0; i < NPROC; i++)
    log->completed[i] = 0;
  for(i = 0; i < 3; i++)
    log->active[i] = 0;
  log->overrun = 0;

  sem_init(PRINTLOCK, 1);
  sem_init(10 + PRINTER, capacity[PRINTER]);
  sem_init(10 + SCANNER, capacity[SCANNER]);
  sem_init(10 + DISK, capacity[DISK]);

  for(i = 0; i < NPROC; i++){
    pid = fork();
    if(pid == 0){
      shmget();
      worker(log, i);
      exit();
    }
  }
  for(i = 0; i < NPROC; i++)
    wait();

  printf(1, "\nCompletion counts:");
  for(i = 0; i < NPROC; i++)
    printf(1, " P%d=%d", i, log->completed[i]);
  printf(1, "\n");

  int all_done = 1;
  for(i = 0; i < NPROC; i++)
    if(log->completed[i] != CYCLES)
      all_done = 0;

  if(all_done && !log->overrun)
    printf(1, "SYNCDEADLOCK PASS: all processes finished, no resource overrun, no deadlock\n");
  else if(log->overrun)
    printf(1, "SYNCDEADLOCK FAIL: a resource pool exceeded its capacity\n");
  else
    printf(1, "SYNCDEADLOCK FAIL: not all processes completed all cycles\n");

  exit();
}
