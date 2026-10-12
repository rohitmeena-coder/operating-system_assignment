#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

volatile int counter;
void worker(void *arg) { int n=*(int*)arg; for(int i=0;i<100000;i++) counter++; printf("thread %d done\n",n); exit(0); }
int main(void) {
  counter=0; int ids[3]={1,2,3};
  for(int i=0;i<3;i++){ void *s=malloc(4096); if(clone(worker,&ids[i],s+4096)<0) printf("clone failed\n"); }
  for(int i=0;i<3;i++) join();
  printf("threadtest: final counter=%d (race-prone; expected upper bound 300000)\n",counter);
  exit(0);
}
