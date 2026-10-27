#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  int i, pid, status;
  char *base;

  base = sbrk(32 * 4096);
  if(base == (char*)-1){
    printf(2, "rohit faulttest: sbrk failed\n");
    exit();
  }
  for(i = 0; i < 5; i++)
    base[i * 3 * 4096] = (char)(i + 1);
  printf(1, "ROHIT FAULTTEST: reserved=32 pages touched=5 faults=%d (expected 5)\n",
         getfaults());

  pid = fork();
  if(pid == 0){
    /* Deliberately outside proc->sz: only this child must be killed. */
    base[32 * 4096] = 99;
    printf(1, "ROHIT FAULTTEST: invalid access unexpectedly survived\n");
    exit();
  }
  wait(&status);
  printf(1, "ROHIT FAULTTEST: invalid child terminated, parent alive, faults=%d\n",
         getfaults());
  if(getfaults() == 5)
    printf(1, "ROHIT FAULTTEST PASS\n");
  else
    printf(1, "ROHIT FAULTTEST FAIL\n");
  exit();
}
