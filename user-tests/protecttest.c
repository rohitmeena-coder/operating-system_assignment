#include "types.h"
#include "stat.h"
#include "user.h"
int main(void){int p=fork();if(p==0){printf(1,"protecttest: invalid access should be killed by xv6\n");*(int*)0x7fffffff=1;exit();}wait();printf(1,"protecttest parent survived child fault\n");exit();}
