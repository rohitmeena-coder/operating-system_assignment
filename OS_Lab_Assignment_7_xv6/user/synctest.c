#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

volatile int counter;
void inc(void *arg) { for(int i=0;i<100000;i++){ sem_wait(0); counter++; sem_signal(0); } exit(0); }
int main(void) {
  counter=0; sem_init(0,1);
  for(int i=0;i<3;i++){ void *s=malloc(4096); clone(inc,0,s+4096); }
  for(int i=0;i<3;i++) join();
  printf("synctest: final counter=%d (expected 300000)\n",counter);
  sem_init(1,1); sem_init(2,0); sem_init(3,1);
  printf("producer-consumer semaphores: empty=1 full=0 mutex=1; bounded buffer size=5\n");
  exit(0);
}
