#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  int before, after;

  printf(1, "ROHIT PTTEST: initial page table\n");
  before = vmprint();
  printf(1, "ROHIT PTTEST: initial valid user pages=%d\n", before);

  if(sbrk(4 * 4096) == (char*)-1){
    printf(2, "pttest: sbrk failed\n");
    exit();
  }
  printf(1, "ROHIT PTTEST: after sbrk(4 * 4096)\n");
  after = vmprint();
  printf(1, "ROHIT PTTEST: new valid user pages=%d (expected 4)\n", after - before);
  if(after - before == 4)
    printf(1, "ROHIT PTTEST PASS\n");
  else
    printf(1, "ROHIT PTTEST FAIL\n");
  exit();
}
