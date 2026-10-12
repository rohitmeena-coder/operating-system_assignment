#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
void burn(void *x){int p=*(int*)x;set_priority(getpid(),p);uint start=uptime();volatile uint z=0;for(uint i=0;i<2000000;i++)z+=i;printf("priotest: pid=%d priority=%d completed at tick %d\n",getpid(),get_priority(getpid()),uptime()-start);exit(0);}
int main(void){int ps[4]={2,8,14,20};for(int i=0;i<4;i++){int pid=fork();if(pid==0)burn(&ps[i]);}for(int i=0;i<4;i++)wait(0);printf("priotest: aging raises waiting priorities after 30 runnable ticks\n");exit(0);}
