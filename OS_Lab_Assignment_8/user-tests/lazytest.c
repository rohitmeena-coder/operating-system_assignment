#include "types.h"
#include "stat.h"
#include "user.h"
int main(void){struct meminfo a,b;sbrk(32*1024*1024);getmeminfo(&a);printf(1,"lazy reserved: free=%d user=%d\n",a.free_pages,a.user_pages);char *p=sbrk(0);p[0]=1;p[16*1024*1024]=2;p[31*1024*1024]=3;getmeminfo(&b);printf(1,"lazy touched: free=%d user=%d\n",b.free_pages,b.user_pages);int c=fork();if(c==0){char buf[32];write(1,buf,0);exit();}wait();printf(1,"lazy fork/reap complete\n");exit();}
