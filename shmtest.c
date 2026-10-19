#include "types.h"
#include "stat.h"
#include "user.h"
int main(void){int *p=(int*)sbrk(4096);*p=42;int f=fork();if(f==0){*p=99;printf(1,"shmtest: fork uses independent copied page value=%d\n",*p);exit();}wait();printf(1,"shmtest: parent value=%d\n",*p);exit();}
