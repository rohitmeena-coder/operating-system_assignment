// Assignment 10, Q3 - pagetest.c
// User program that exercises pageinfo() and setreadonly():
//   1. allocate one page with sbrk(),
//   2. show its page-table information (flags P=1, W=1, U=1),
//   3. write to it (works - page is writable),
//   4. make the page read-only with setreadonly(),
//   5. show the page-table information again (W must now be 0),
//   6. try to write again -> page fault -> the kernel kills the process.
#include "types.h"
#include "stat.h"
#include "user.h"

#define PGSIZE 4096

int
main(void)
{
  char *page;

  page = sbrk(PGSIZE);
  if(page == (char*)-1){
    printf(1, "pagetest: sbrk failed\n");
    exit();
  }

  printf(1, "pagetest: allocated one page at va 0x%x\n", page);
  printf(1, "pagetest: page info BEFORE setreadonly:\n");
  pageinfo((int)page);

  *page = 'A';
  printf(1, "pagetest: wrote '%c' to the page (write allowed)\n", *page);

  if(setreadonly((int)page) < 0){
    printf(1, "pagetest: setreadonly failed\n");
    exit();
  }
  printf(1, "pagetest: page is now READ-ONLY\n");
  printf(1, "pagetest: page info AFTER setreadonly:\n");
  pageinfo((int)page);

  printf(1, "pagetest: attempting to write to the read-only page...\n");
  printf(1, "pagetest: (a page fault should occur and xv6 will kill this process)\n");
  *(volatile char*)page = 'B';      // <-- triggers a page fault

  // Never reached: the write above faults and the process is killed.
  printf(1, "pagetest: ERROR - write to read-only page succeeded!\n");
  exit();
}
