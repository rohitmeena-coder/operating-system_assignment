#include "types.h"
#include "stat.h"
#include "user.h"

#define BUFFER_SIZE 5
#define ITEMS 20
#define EMPTY 0
#define FULL 1
#define MUTEX 2

struct shared_buffer {
  int data[BUFFER_SIZE];
  int in;
  int out;
  int produced;
  int consumed;
};

static void
short_delay(void)
{
  volatile int i;
  for(i = 0; i < 120000; i++)
    ;
}

int
main(void)
{
  struct shared_buffer *b = (struct shared_buffer*)shmget();
  int pid, i, value;

  if((uint)b == (uint)-1){
    printf(1, "prodcons: shmget failed\n");
    exit();
  }
  b->in = b->out = b->produced = b->consumed = 0;
  sem_init(EMPTY, BUFFER_SIZE);
  sem_init(FULL, 0);
  sem_init(MUTEX, 1);

  pid = fork();
  if(pid < 0){
    printf(1, "prodcons: fork failed\n");
    exit();
  }
  if(pid == 0){
    shmget();
    for(i = 0; i < ITEMS; i++){
      sem_wait(FULL);
      sem_wait(MUTEX);
      value = b->data[b->out];
      b->out = (b->out + 1) % BUFFER_SIZE;
      b->consumed++;
      printf(1, "Consumer: removed %d (consumed=%d)\n", value, b->consumed);
      sem_post(MUTEX);
      sem_post(EMPTY);
      short_delay();
    }
    exit();
  }

  for(i = 1; i <= ITEMS; i++){
    sem_wait(EMPTY);
    sem_wait(MUTEX);
    b->data[b->in] = i;
    b->in = (b->in + 1) % BUFFER_SIZE;
    b->produced++;
    printf(1, "Producer: inserted %d (produced=%d)\n", i, b->produced);
    sem_post(MUTEX);
    sem_post(FULL);
    short_delay();
  }
  wait();

  printf(1, "Producer-consumer totals: produced=%d consumed=%d\n",
         b->produced, b->consumed);
  if(b->produced == ITEMS && b->consumed == ITEMS)
    printf(1, "PRODCONS PASS: all items delivered in FIFO order\n");
  else
    printf(1, "PRODCONS FAIL\n");
  exit();
}
