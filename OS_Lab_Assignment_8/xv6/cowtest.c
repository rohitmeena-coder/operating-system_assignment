#include "types.h"
#include "stat.h"
#include "user.h"
int main(void){int x=1,p=fork();if(p==0){x=2;printf(1,"cow child x=%d\n",x);exit();}wait();printf(1,"cow parent x=%d\n",x);exit();}
