#include "types.h"
#include "stat.h"
#include "user.h"
int main(void){printf(1,"vmprint initial\n");vmprint();sbrk(8*4096);printf(1,"vmprint after grow\n");vmprint();sbrk(-4096);printf(1,"vmprint after shrink\n");vmprint();exit();}
