#include "types.h"
#include "stat.h"
#include "user.h"
int global;
int main(void){int local;char *heap=malloc(4096);printf(1,"global va=%p pa=0x%x\n",&global,va2pa((int)&global));printf(1,"local va=%p pa=0x%x\n",&local,va2pa((int)&local));printf(1,"heap va=%p pa=0x%x\n",heap,va2pa((int)heap));printf(1,"unmapped pa=0x%x\n",va2pa(0x7fffffff));int p=fork();if(p==0){printf(1,"child global va=%p pa=0x%x\n",&global,va2pa((int)&global));exit();}printf(1,"parent global va=%p pa=0x%x\n",&global,va2pa((int)&global));wait();exit();}
