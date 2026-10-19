#include "types.h"
#include "stat.h"
#include "user.h"
void show(char *s){struct meminfo m;if(getmeminfo(&m)<0){printf(1,"getmeminfo failed\n");return;}printf(1,"%s total=%d free=%d user=%d pagetables=%d\n",s,m.total_pages,m.free_pages,m.user_pages,m.pagetable_pages);}
int main(void){show("before");char *p=sbrk(8192);p[0]=1;p[4096]=2;show("after heap");int x=fork();if(x==0){show("child");exit();}wait();show("after reap");exit();}
